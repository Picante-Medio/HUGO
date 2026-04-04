#include "SceneManager.h"
#include "LuaBinder.h"
#include "Scene.h"
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
    LuaBinder::getInstance()->registerMethod<SceneManager, void, HUGO::String>(*this, &SceneManager::setCurrentSceneLUA, "SetCurrentScene");

    m_physicsModule = physicsModule;

    return m_chatParser.readChatFolder(chatFolder);
}

ErrorOrVoid SceneManager::loadScene(const HUGO::String& sceneName){
    ErrorOrType<HUGO::SceneData> sceneData = m_chatParser.getSceneData(sceneName);

    if(sceneData.isError()) return ErrorOrVoid(StringError{"Error loading scene: " + sceneData.getError().errorMessage});

    //Create sceneManager from OgreModule

    OgreModule::Instance().createSceneManager(sceneName);

    /*
    -> Create scene

    -> Create systems based on strings

    -> Create entities with components
    */
    m_sceneMap[sceneName].scene = std::make_shared<Scene>(m_input, *this);
    m_sceneMap[sceneName].needsInit = true;

    for(auto systemName : sceneData.getValue().systems){
        ErrorOrType<std::shared_ptr<HUGO::System>> value = HUGO::SystemFactory::makeSystem(m_sceneMap[sceneName].scene, systemName);

        if(value.isError()) {
            value = HUGO::GameLibraryLoader::instance().GenerateAndGetSystem(m_sceneMap[sceneName].scene, systemName);
            if (value.isError()) {
                return ErrorOrVoid(value.getError());
            }
        }

        m_sceneMap[sceneName].scene->addSystem(value.getValue());
    }

    
    for(auto entityData: sceneData.getValue().entities){
        EntityID currEntity = m_sceneMap[sceneName].scene->createEntity();

        for(auto componentData : entityData){
            ErrorOrVoid compResult = HUGO::ComponentFactory::makeComponent(m_sceneMap[sceneName].scene, currEntity, 
                componentData.componentName, componentData.parameters);

            if (compResult.isError()) {
                compResult = HUGO::GameLibraryLoader::instance().GenerateComponent(m_sceneMap[sceneName].scene, currEntity, 
                    componentData.componentName, componentData.parameters);
                if (compResult.isError()) {
                    return ErrorOrVoid(compResult.getError());
                }
            }

        }
    }


    //When no errors occurred, we add the scene to the physics simulation
    m_physicsModule->createSceneSimulation(sceneName, m_sceneMap[sceneName].scene);

    return ErrorOrVoid();
}

ErrorOrVoid SceneManager::unloadScene(const HUGO::String& sceneName){
    if(m_sceneMap.find(sceneName) == m_sceneMap.end()) return ErrorOrVoid(StringError{"Scene " + sceneName + " can't be unloaded because it doesn't exist"});

    m_sceneMap.erase(sceneName);


    //Destroy sceneManager associated with scene as it is unloaded
    OgreModule::Instance().destroySceneManager(sceneName);

    return ErrorOrVoid();
}

void SceneManager::setCurrentSceneLUA(const HUGO::String sceneName){
    setCurrentScene(sceneName);
}

void SceneManager::setCurrentScene(const HUGO::String& sceneName){
    needsSceneChanged = true;
    nextSceneName = sceneName;
}

void SceneManager::sceneChangeRequested(){
    //If not requested, return.
    if(!needsSceneChanged) return;
    needsSceneChanged = false;

    //If requested scene is not loaded in the map, load it
    if(m_sceneMap.find(nextSceneName) == m_sceneMap.end()){
        ErrorOrVoid err = loadScene(nextSceneName);

        //Print error in lua console
        if(err.isError()){
            /* It should call Lua console to show the error */
            m_currentScene.lock()->log(std::shared_ptr<Error>(new StringError(err.getError())));
        }
    }

    //Sets current sceneManager for the new one.
    OgreModule::Instance().setCurrentSceneManager(nextSceneName);

    m_currentScene = m_sceneMap[nextSceneName].scene;

    if(m_sceneMap[nextSceneName].needsInit){
        m_currentScene.lock()->init();
        m_sceneMap[nextSceneName].needsInit = false;
    }
    
    m_currentScene.lock()->load();
}

ErrorOrVoid SceneManager::setMainScene(){
    ErrorOrType<HUGO::String> err = m_chatParser.getMainSceneName();

    if (err.isError()) return ErrorOrVoid(err.getError());

    setCurrentScene(err.getValue());

    return ErrorOrVoid();

}

void SceneManager::updateCurrentScene(double dt){
    m_currentScene.lock()->update(dt);
}

void SceneManager::fixedUpdateCurrentScene(double dt){
    m_currentScene.lock()->fixedUpdate(dt);
}

void SceneManager::dispatchMessagesCurrentScene() {
    m_currentScene.lock()->dispatchMessages();
}
