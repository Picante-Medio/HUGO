#include "SceneManager.h"
#include "LuaBinder.h"
#include "InternalScene.h"
#include "SystemFactory.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "IPhysicsModule.h"
#include "IInput.h"
#include "OgreModule.h"
#include "ParentComponent.h"

#include "GameLibraryLoader.h"
#include <memory>

using namespace HUGO;

SceneManager::SceneManager(std::shared_ptr<HUGO::IInput> inputModule): m_chatParser(), m_sceneMap(), m_input(inputModule) {
}

HUGO::ErrorOrVoid SceneManager::initializeParser(const HUGO::String& chatFolder, std::shared_ptr<IPhysicsModule> physicsModule){

    //Must be called after initialization of LuaBinder
    LuaBinder::getInstance().registerMethod<SceneManager, void, HUGO::String>(*this, &SceneManager::setCurrentSceneLUA, "SetCurrentScene");

    m_physicsModule = physicsModule;

    return m_chatParser.readChatFolder(chatFolder);
}

HUGO::ErrorOrType<EntityID> createEntity(std::shared_ptr<InternalScene> scene, const EntityData& entityData){
    
    EntityID currEntity = scene->createEntity();

        //======Creates components========
        for(auto componentData : entityData.components){
            HUGO::ErrorOrVoid compResult = HUGO::ComponentFactory::makeComponent(scene, currEntity, 
                componentData.componentName, componentData.parameters);

            if (compResult.isError())
                return HUGO::ErrorOrType<EntityID>(compResult.getError());
        }

        //Iterate through children. Creates the child with components, then parents them with currentEntity. This is donde recursively
        for(auto childrenData : entityData.children){
            HUGO::ErrorOrType<EntityID> err = createEntity(scene, childrenData);
            if(err.isError()) return err;
            EntityID childID = err.getValue();

            scene->addComponent<ParentComponent>(childID, [currEntity](ParentComponent& cmp){
                cmp.parent = currEntity;
            });
        }

    return HUGO::ErrorOrType<EntityID>(currEntity);
}

HUGO::ErrorOrVoid SceneManager::loadScene(const HUGO::String& sceneName){
    HUGO::ErrorOrType<HUGO::SceneData> sceneData = m_chatParser.getSceneData(sceneName);

    if(sceneData.isError()) return HUGO::ErrorOrVoid(HUGO::StringError{"Error loading scene: " + sceneData.getError().errorMessage});

    //Create sceneManager from OgreModule

    OgreModule::Instance().createSceneManager(sceneName);

    //Momentarialy sets Ogre::SceneManager to the loaded scene, then it is returned to the currentSceneManager
    OgreModule::Instance().setCurrentSceneManager(sceneName);

    /*
    -> Create scene

    -> Create systems based on strings

    -> Create entities with components
    */
    m_sceneMap[sceneName] = std::make_shared<InternalScene>(m_input, *this);

    for(auto systemName : sceneData.getValue().systems){
        HUGO::ErrorOrType<std::shared_ptr<HUGO::System>> value = HUGO::SystemFactory::makeSystem(*m_sceneMap[sceneName], systemName);

        if(value.isError())
            return HUGO::ErrorOrVoid(value.getError());

        m_sceneMap[sceneName]->addSystem(value.getValue());
        value.getValue()->init();
    }

    
    for(auto entityData: sceneData.getValue().entities){
        createEntity(m_sceneMap[sceneName], entityData);
    }

    OgreModule::Instance().setCurrentSceneManager(m_currentSceneName);


    //When no errors occurred, we add the scene to the physics simulation
    m_physicsModule->createSceneSimulation(sceneName, m_sceneMap[sceneName]);

    return HUGO::ErrorOrVoid();
}

HUGO::ErrorOrVoid SceneManager::unloadScene(const HUGO::String& sceneName){
    if(!m_sceneMap.count(sceneName)) return HUGO::ErrorOrVoid(HUGO::StringError{"Scene " + sceneName + " can't be unloaded because it doesn't exist"});

    m_physicsModule->destroySceneSimulation(sceneName);
    m_sceneMap.erase(sceneName);

    //Destroy sceneManager associated with scene as it is unloaded
    OgreModule::Instance().destroySceneManager(sceneName);

    return HUGO::ErrorOrVoid();
}

void SceneManager::setCurrentSceneLUA(const HUGO::String sceneName){
    setCurrentScene(sceneName);
}

void SceneManager::setCurrentScene(const HUGO::String& sceneName){
    m_sceneChangeInfo.needsSceneChanged = true;
    m_sceneChangeInfo.nextSceneName = sceneName;
    m_sceneChangeInfo.destroyPreviousScene = false;
}

void SceneManager::setCurrentSceneAndDestroyPrevious(const HUGO::String& sceneName){
    m_sceneChangeInfo.needsSceneChanged = true;
    m_sceneChangeInfo.nextSceneName = sceneName;
    m_sceneChangeInfo.destroyPreviousScene = true;
}

HUGO::ErrorOrVoid SceneManager::sceneChangeRequested(){
    //If not requested, return.
    if(!m_sceneChangeInfo.needsSceneChanged) return HUGO::ErrorOrVoid();
        m_sceneChangeInfo.needsSceneChanged = false;
    if(m_sceneChangeInfo.destroyPreviousScene) unloadScene(m_currentSceneName);
    else m_physicsModule->pauseSceneSimulation(m_currentSceneName);

    //If requested scene is not loaded in the map, load it
    if(!m_sceneMap.count(m_sceneChangeInfo.nextSceneName)){
        HUGO::ErrorOrVoid err = loadScene(m_sceneChangeInfo.nextSceneName);

        //Print error in lua console
        if(err.isError()){
            return err;
        }
    }

    //Sets current sceneManager for the new one.
    OgreModule::Instance().setCurrentSceneManager(m_sceneChangeInfo.nextSceneName);
    m_physicsModule->resumeSceneSimulation(m_sceneChangeInfo.nextSceneName);

    m_currentScene = m_sceneMap[m_sceneChangeInfo.nextSceneName];
    m_currentSceneName = m_sceneChangeInfo.nextSceneName;
    
    m_currentScene.lock()->load();

    return HUGO::ErrorOrVoid();
}

void SceneManager::updateCurrentScene(uint32_t dt){
    m_currentScene.lock()->update(dt);
}

void SceneManager::fixedUpdateCurrentScene(uint32_t dt){
    m_currentScene.lock()->fixedUpdate(dt);
}

void SceneManager::dispatchMessagesCurrentScene() {
    m_currentScene.lock()->dispatchMessages();
}

HUGO::ErrorOrType<EntityID> SceneManager::instantiatePrefab(const HUGO::String& prefabName, std::shared_ptr<InternalScene> scene){
    HUGO::ErrorOrType<EntityData> prefabData = m_chatParser.getPrefabData(prefabName);
    
    if(prefabData.isError()) return HUGO::ErrorOrType<EntityID>(prefabData.getError().description());
    
    return createEntity(scene, prefabData.getValue());
}

void HUGO::SceneManager::setAmbientLight(const HUGO::Vector3 &l)
{
    OgreModule::Instance().setAmbientLight(l);
}
