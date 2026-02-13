#pragma once

#include "btBulletDynamicsCommon.h"

namespace HUGO {
    struct Vector3;
    struct Quaternion;
}

struct BulletUtils {
public:
    static btVector3 vector3ToBullet(const HUGO::Vector3& vec3);
    static HUGO::Vector3 btVector3ToVector3(const btVector3& vec3);

    static btQuaternion quaternionToBullet(const HUGO::Quaternion& q);
    static HUGO::Quaternion btQuaternionToQuaternion(const btQuaternion& q);
};
