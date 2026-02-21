#pragma once
#include <memory>
#include "OgreRenderWindow.h"
#include "OgreRoot.h"

namespace HUGO{
    class Core{
    public:
        Core();
        void init();
        void destroy();
    
        void run();
    
    private:
    
        void initModules();
    
        void initOgre();
        void destroyOgre();
    
        std::shared_ptr<Ogre::Root> m_ogreRoot;
        Ogre::RenderWindow* m_renderWindow;
    };

}