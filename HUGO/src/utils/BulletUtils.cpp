#include "BulletUtils.h"
#include "HugoMath.h"

btVector3 BulletUtils::vector3ToBullet(const HUGO::Vector3& vec3) {
    return btVector3(vec3.x, vec3.y, vec3.z);
}

HUGO::Vector3 BulletUtils::btVector3ToVector3(const btVector3& vec3) {
    return HUGO::Vector3(vec3.getX(), vec3.getY(), vec3.getZ());
}

btQuaternion BulletUtils::quaternionToBullet(const HUGO::Quaternion& q) {
    return btQuaternion(q.x, q.y, q.z, q.w);
}

HUGO::Quaternion BulletUtils::btQuaternionToQuaternion(const btQuaternion& q) {
    return HUGO::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}