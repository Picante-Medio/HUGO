#include "Ogre.h"
#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "InitOgre.h"

namespace HUGO{

    std::pair<std::shared_ptr<Ogre::Root>, Ogre::RenderWindow*> initializeOgre(){
        std::shared_ptr<Ogre::Root> mRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");

        mRoot->showConfigDialog(NULL);

        Ogre::RenderWindow* mRenderWindow = mRoot->initialise(true, "Prueba Ogre");

        // mRoot->startRendering();
        return std::make_pair(mRoot, mRenderWindow);
    }
}

