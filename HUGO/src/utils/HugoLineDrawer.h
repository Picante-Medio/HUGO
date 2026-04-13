#pragma once

//#include <OgreManualObject.h>
#include "HugoMath.h"

namespace HUGO{
    struct lineDrawer{

        //Ogre::ManualObject* mLines;

        void draw(Vector3 from, Vector3 to, Vector3 color);
    };
}