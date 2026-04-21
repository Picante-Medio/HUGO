#include "SceneFactory.h"
#include "Scene.h"

using namespace HUGO;

SceneFactory::SceneFactory(): m_chatParser(ChatParser()){
}

void SceneFactory::initialize(HUGO::String chatFolder){

    m_chatParser.readChatFolder(chatFolder);
}


void SceneFactory::populateScene(std::shared_ptr<Scene> scene, HUGO::String sceneName){
    SceneData sceneData = m_chatParser.getSceneData(sceneName);

    for(auto entity : sceneData.entities){
        EntityID createdEnt = scene->createEntity();

        for(auto component : entity){
            //Creas el componente con los parametros
        }
    }
}