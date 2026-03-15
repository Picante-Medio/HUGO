#pragma once
#include "Ogre.h"
#include "HugoError.h"

namespace HUGO {
    struct Vector3;
    struct Quaternion;
    struct Radian;
    struct Degree;
}

namespace Ogre {
    class Overlay;
    class OverlayElement;
    class OverlayContainer;
}

struct OgreUtils {
public:
    static Ogre::Vector3 vector3ToOgre(const HUGO::Vector3& vec3);
    static Ogre::Quaternion quaternionToOgre(const HUGO::Quaternion& q);
    static Ogre::Radian radianToOgre(const HUGO::Radian& radian);
    static Ogre::Degree degreeToOgre(const HUGO::Degree& degree);
};

struct OgreOverlayUtils{
    static ErrorOrType<Ogre::Overlay*> getOverlay(const HUGO::String& name);
    static ErrorOrType<Ogre::OverlayElement*> getElement(const HUGO::String& name);
    static ErrorOrType<Ogre::OverlayContainer*> getContainer(const HUGO::String& name);
};
