#include "SceneFactory.h"
#include "Scene.h"

using namespace HUGO;

SceneFactory::SceneFactory(): m_chatParser(ChatParser()){
}

void SceneFactory::initialize(HUGO::String chatFolder){

    m_chatParser.readChatFolder(chatFolder);
}


void SceneFactory::populateScene(std::shared_ptr<Scene> scene, HUGO::String sceneName){
    
}