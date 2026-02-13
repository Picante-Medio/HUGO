#include "HugoMath.h"
#include "OgreUtils.h"

Ogre::Vector3 OgreUtils::vector3ToOgre(const HUGO::Vector3 &vec3)
{
    return Ogre::Vector3(vec3.x, vec3.y, vec3.z);
}

Ogre::Quaternion OgreUtils::quaternionToOgre(const HUGO::Quaternion &q)
{
    return Ogre::Quaternion(q.w, q.x, q.y, q.z);
}

Ogre::Radian OgreUtils::radianToOgre(const HUGO::Radian &radian)
{
    return Ogre::Radian(radian.rad);
}
Ogre::Degree OgreUtils::degreeToOgre(const HUGO::Degree &degree)
{
    return Ogre::Degree(degree.deg);
}
