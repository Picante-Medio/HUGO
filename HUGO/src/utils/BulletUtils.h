#pragma once

// #include "HugoMath.h"
#include "btBulletDynamicsCommon.h"

namespace HUGO {
    struct Vector3;
    struct Quaternion;
}

struct BulletUtils {
public:
    static btVector3 vector3ToBullet(const HUGO::Vector3& vec3);
    static btQuaternion quaternionToBullet(const HUGO::Quaternion& q);
};
