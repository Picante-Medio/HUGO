#pragma once
#include "ChatParser.h"
#include "HugoTypes.h"

class Scene;

namespace HUGO{
    
    class SceneFactory{
        ChatParser m_chatParser;
        public:
        //Initialize must be called after this
        SceneFactory();
        
        //Must be initialized with the path to all the .chats of the game
        void initialize(HUGO::String chatFolder);

        //Adds to the scene the entity data and systems held in the parser
        void populateScene(std::shared_ptr<Scene> scene, HUGO::String sceneName);
    };
}
