#include "Component.h"
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"

enum class RBShapes{
    Box,
    Sphere,
    Capsule,
    Cylinder,
    Cone
};

struct RigidBodyComponent: public Component
{
    float mass;
    float damping;
    HUGO::Vector3 positionOffset;
    HUGO::Quaternion rotationOffset;

    HUGO::Vector3 linearVelocity;
    HUGO::Vector3 angularVelocity;

    // Forces can be applied throught rigidbody or external components to query which rigidbodies have force to be applied.
    // For this we need to query every rigidbody and a way to filter if the entity attached has or not the force component.
    // Since there's no way to do so at the moment, we choose the first option 

    HUGO::Vector3 forceToApplyThisFrame;
    HUGO::Vector3 torqueToApplyThisFrame;

    HUGO::Vector3 impulseToApplyThisFrame;
    HUGO::Vector3 impulseTorqueToApplyThisFrame;

    RigidBodyComponent(EntityID entity) 
        : Component(entity){};
    
    friend class PhysicsSystem;
private:
    HUGO::OgreObjectID objectID = HUGO::INVALID_OGRE_OBJECT_ID;
};