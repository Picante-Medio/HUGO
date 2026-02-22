#pragma once
#include <memory>
#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "HugoVirtualTimer.h"
#include "miniaudio.h"

class Scene;
class VirtualTimer;

namespace HUGO{
    class Core{
    public:
        Core();
        void init();
        void destroy();
    
        void run();
    
    private:
    
        void initModules();
    
        //Ogre
        void initOgre();
        void destroyOgre();
    
        std::shared_ptr<Ogre::Root> m_ogreRoot;
        Ogre::RenderWindow* m_renderWindow;

        //Miniaudio
        void initMiniaudio();
        std::shared_ptr<ma_engine> engine;

        VirtualTimer vt;

        std::shared_ptr<Scene> scenePtr;
    };

}