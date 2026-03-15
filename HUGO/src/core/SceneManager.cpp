#include "SceneManager.h"
#include "LuaBinder.h"
#include "Scene.h"
#include "SystemFactory.h"
#include "ComponentRegistry.h"

SceneManager::SceneManager(): m_chatParser(), m_sceneMap(){
    LuaBinder& luaBinder = LuaBinder::Instance();
    luaBinder.registerMethod<SceneManager, void, HUGO::String>(*this, &SceneManager::setCurrentSceneLUA, "SetCurrentScene");
}

ErrorOrVoid SceneManager::initializeParser(const HUGO::String& chatFolder){
    return m_chatParser.readChatFolder(chatFolder);
}

ErrorOrVoid SceneManager::loadScene(const HUGO::String& sceneName){
    ErrorOrType<HUGO::SceneData> sceneData = m_chatParser.getSceneData(sceneName);

    if(sceneData.isError()) return ErrorOrVoid(StringError{"Error loading scene: " + sceneData.getError().errorMessage});

    /*
    -> Create scene

    -> Create systems based on strings

    -> Create entities with components
    */
    m_sceneMap[sceneName] = std::make_shared<Scene>();

    for(auto systemName : sceneData.getValue().systems){
        ErrorOrType<std::shared_ptr<System>> value = SystemFactory::makeSystem(m_sceneMap[sceneName], systemName);

        if(value.isError()) return ErrorOrVoid(value.getError());

        m_sceneMap[sceneName]->addSystem(value.getValue());
        value.getValue()->init();
    }

    /*
    for(auto entityData: sceneData.getValue().entities){
        EntityID currEntity = m_sceneMap[sceneName]->createEntity();

        for(auto componentData : entityData){
            
        }
    }
*/

    return ErrorOrVoid();
}

ErrorOrVoid SceneManager::unloadScene(const HUGO::String& sceneName){
    if(m_sceneMap.find(sceneName) == m_sceneMap.end()) return ErrorOrVoid(StringError{"Scene " + sceneName + " can't be unloaded because it doesn't exist"});

    m_sceneMap.erase(sceneName);

    return ErrorOrVoid();
}

void SceneManager::setCurrentSceneLUA(const HUGO::String sceneName){
    setCurrentScene(sceneName);
}

void SceneManager::setCurrentScene(const HUGO::String& sceneName){

    //If requested scene is not loaded in the map, load it
    if(m_sceneMap.find(sceneName) == m_sceneMap.end()){
        ErrorOrVoid err = loadScene(sceneName);

        //Print error in lua console
        if(err.isError()){
            /* It should call Lua console to show the error */
        }
    }

    m_currentScene = m_sceneMap[sceneName];
}

void SceneManager::updateCurrentScene(double dt){
    m_currentScene->update(dt);
}
