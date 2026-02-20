#pragma once
#include <OgreRoot.h>

namespace HUGO
{
    std::pair<std::shared_ptr<Ogre::Root>, Ogre::RenderWindow*> initializeOgre();
}