#pragma once
#include <memory>
#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "HugoVirtualTimer.h"
#include "miniaudio.h"
class Scene;
class VirtualTimer;
namespace HUGO {
    class Core {
        void initModules();

        //Ogre
        void initOgre();
        void destroyOgre();

        std::shared_ptr<Ogre::Root> m_ogreRoot;
        Ogre::RenderWindow* m_renderWindow;

        //Miniaudio
        void initMiniaudio();
        void destroyMiniaudio();
        std::shared_ptr<ma_engine> maEngine;


        VirtualTimer vt;

        std::shared_ptr<Scene> scenePtr;
    public:
        Core();
        void init();
        void destroy();

        void run();
    };

}
