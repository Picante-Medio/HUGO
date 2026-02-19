#pragma once
#include <OgreRoot.h>

namespace HUGO
{
    struct OgreContext {
        std::shared_ptr<Ogre::Root> root;
        Ogre::RenderWindow* window;
    };

    std::pair<std::shared_ptr<Ogre::Root>, Ogre::RenderWindow*> initializeOgre();
}