#include "BulletUtils.h"
#include "HugoMath.h"

btVector3 BulletUtils::vector3ToBullet(const HUGO::Vector3& vec3) {
    return btVector3(vec3.x, vec3.y, vec3.z);
}

btQuaternion BulletUtils::quaternionToBullet(const HUGO::Quaternion& q) {
    return btQuaternion(q.x, q.y, q.z, q.w);
}