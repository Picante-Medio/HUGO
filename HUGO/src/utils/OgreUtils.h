#pragma once
#include "Ogre.h"
namespace HUGO {
    struct Vector3;
    struct Quaternion;
    struct Radian;
    struct Degree;
}

struct OgreUtils {
public:
    static Ogre::Vector3 vector3ToOgre(const HUGO::Vector3& vec3);
    static Ogre::Quaternion quaternionToOgre(const HUGO::Quaternion& q);
    static Ogre::Radian radianToOgre(const HUGO::Radian& radian);
    static Ogre::Degree degreeToOgre(const HUGO::Degree& degree);
};
