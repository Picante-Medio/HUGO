#include "SceneManager.h"
#include "LuaBinder.h"
#include "InternalScene.h"
#include "SystemFactory.h"
#include "ComponentFactory.h"
#include "ComponentRegistry.h"
#include "IPhysicsModule.h"
#include "IInput.h"
#include "OgreModule.h"

#include "GameLibraryLoader.h"
#include <memory>

using namespace HUGO;

SceneManager::SceneManager(std::shared_ptr<HUGO::IInput> inputModule): m_chatParser(), m_sceneMap(), m_input(inputModule) {
}

ErrorOrVoid SceneManager::initializeParser(const HUGO::String& chatFolder, std::shared_ptr<IPhysicsModule> physicsModule){

    //Must be called after initialization of LuaBinder
    LuaBinder::getInstance().registerMethod<SceneManager, void, HUGO::String>(*this, &SceneManager::setCurrentSceneLUA, "SetCurrentScene");

    m_physicsModule = physicsModule;

    return m_chatParser.readChatFolder(chatFolder);
}

ErrorOrType<EntityID> createEntity(std::shared_ptr<InternalScene> scene, const EntityData& entityData){
    
    EntityID currEntity = scene->createEntity();

        for(auto componentData : entityData.components){
            ErrorOrVoid compResult = HUGO::ComponentFactory::makeComponent(scene, currEntity, 
                componentData.componentName, componentData.parameters);

            if (compResult.isError())
                return ErrorOrType<EntityID>(compResult.getError());
        }

        //Parenting is missing

    return ErrorOrType<EntityID>(currEntity);
}

ErrorOrVoid SceneManager::loadScene(const HUGO::String& sceneName){
    ErrorOrType<HUGO::SceneData> sceneData = m_chatParser.getSceneData(sceneName);

    if(sceneData.isError()) return ErrorOrVoid(StringError{"Error loading scene: " + sceneData.getError().errorMessage});

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
        ErrorOrType<std::shared_ptr<HUGO::System>> value = HUGO::SystemFactory::makeSystem(*m_sceneMap[sceneName], systemName);

        if(value.isError())
            return ErrorOrVoid(value.getError());

        m_sceneMap[sceneName]->addSystem(value.getValue());
        value.getValue()->init();
    }

    
    for(auto entityData: sceneData.getValue().entities){
        createEntity(m_sceneMap[sceneName], entityData);
    }

    OgreModule::Instance().setCurrentSceneManager(m_currentSceneName);


    //When no errors occurred, we add the scene to the physics simulation
    m_physicsModule->createSceneSimulation(sceneName, m_sceneMap[sceneName]);

    return ErrorOrVoid();
}

ErrorOrVoid SceneManager::unloadScene(const HUGO::String& sceneName){
    if(m_sceneMap.find(sceneName) == m_sceneMap.end()) return ErrorOrVoid(StringError{"Scene " + sceneName + " can't be unloaded because it doesn't exist"});

    m_physicsModule->destroySceneSimulation(sceneName);
    m_sceneMap.erase(sceneName);

    //Destroy sceneManager associated with scene as it is unloaded
    OgreModule::Instance().destroySceneManager(sceneName);

    return ErrorOrVoid();
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

ErrorOrVoid SceneManager::sceneChangeRequested(){
    //If not requested, return.
    if(!m_sceneChangeInfo.needsSceneChanged) return ErrorOrVoid();
        m_sceneChangeInfo.needsSceneChanged = false;
    if(m_sceneChangeInfo.destroyPreviousScene) unloadScene(m_currentSceneName);

    //If requested scene is not loaded in the map, load it
    if(m_sceneMap.find(m_sceneChangeInfo.nextSceneName) == m_sceneMap.end()){
        ErrorOrVoid err = loadScene(m_sceneChangeInfo.nextSceneName);

        //Print error in lua console
        if(err.isError()){
            return err;
        }
    }

    //Sets current sceneManager for the new one.
    OgreModule::Instance().setCurrentSceneManager(m_sceneChangeInfo.nextSceneName);

    m_currentScene = m_sceneMap[m_sceneChangeInfo.nextSceneName];
    m_currentSceneName = m_sceneChangeInfo.nextSceneName;
    
    m_currentScene.lock()->load();

    return ErrorOrVoid();
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

ErrorOrType<EntityID> SceneManager::instantiatePrefab(const HUGO::String& prefabName, std::shared_ptr<InternalScene> scene){
    ErrorOrType<EntityData> prefabData = m_chatParser.getPrefabData(prefabName);
    
    if(prefabData.isError()) return ErrorOrType<EntityID>(prefabData.getError().description());
    
    return createEntity(scene, prefabData.getValue());
}
