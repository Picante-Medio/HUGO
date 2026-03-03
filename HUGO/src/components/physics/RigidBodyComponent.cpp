#include "RigidBodyComponent.h"

RigidBodyComponent::RigidBodyComponent(EntityID entity, Shape shape)
: Component(entity)
, rbShape(shape)
, flags(0)
{}

#pragma region Setters
// ----------------------Setters----------------------
    
void RigidBodyComponent::setRestitution(float nRestitution)
{
    restitution = nRestitution;
    flags = flags | UpdateFlags::RESTITUTION_UPDATED;
}

void RigidBodyComponent::setFriction(float nFriction)
{
    friction = nFriction;
    flags = flags | UpdateFlags::FRICTION_UPDATED;
}
    
void RigidBodyComponent::setMass(float nMass)
{
    mass = nMass;
    flags = flags | UpdateFlags::MASS_UPDATED;
}
    
void RigidBodyComponent::setDamping(float nDamping)
{
    damping = nDamping;
    flags = flags | UpdateFlags::DAMPING_UPDATED;
}

void RigidBodyComponent::setLinearVelocity(HUGO::Vector3 nVelocity)
{
    linearVelocity = nVelocity;
    flags = flags | UpdateFlags::LINEAR_VELOCITY_UPDATED;
}

void RigidBodyComponent::setAngularVelocity(HUGO::Vector3 nVelocity)
{
    angularVelocity = nVelocity;
    flags = flags | UpdateFlags::ANGULAR_VELOCITY_UPDATED;
}

void RigidBodyComponent::applyImpulse(HUGO::Vector3 nImpulse)
{
    impulseApplied = nImpulse;
    flags = flags | UpdateFlags::IMPULSE_APPLIED;
}
    
void RigidBodyComponent::applyTorqueImpulse(HUGO::Vector3 nImpulse)
{
    torqueImpulseApplied = nImpulse;
    flags = flags | UpdateFlags::TORQUE_IMPULSE_APPLIED;
}
    
void RigidBodyComponent::applyForce(HUGO::Vector3 nForce)
{
    forceApplied = nForce;
    flags = flags | UpdateFlags::FORCE_APPLIED;
}
    
void RigidBodyComponent::applyTorque(HUGO::Vector3 nTorque)
{
    torqueApplied = nTorque;
    flags = flags | UpdateFlags::TORQUE_APPLIED;
}

void RigidBodyComponent::setPositionOffset(HUGO::Vector3 offset)
{
    positionOffset = offset;
    flags = flags | UpdateFlags::POSITION_OFFSET_UPDATED;
}
    
void RigidBodyComponent::setRotationOffset(HUGO::Quaternion offset)
{
    rotationOffset = offset;
    flags = flags | UpdateFlags::ROTATION_OFFSET_UPDATED;
}
#pragma endregion

#pragma region Getters
// ----------------------Getters----------------------

float RigidBodyComponent::getRestitution() const
{
    return restitution;
}
    
float RigidBodyComponent::getFriction() const
{
    return friction;
}
    
float RigidBodyComponent::getMass() const
{
    return mass;
}
    
float RigidBodyComponent::getDamping() const
{
    return damping;
}

HUGO::Vector3 RigidBodyComponent::getLinearVelocity() const
{
    return linearVelocity;
}
    
HUGO::Vector3 RigidBodyComponent::getAngularVelocity() const
{
    return angularVelocity;
}
    
HUGO::Vector3 RigidBodyComponent::getLinearAcceleration() const
{
    return linearAcceleration;
}
    
HUGO::Vector3 RigidBodyComponent::getAngularAcceleration() const
{
    return angularAcceleration;
}

HUGO::Vector3 RigidBodyComponent::getPositionOffset() const
{
    return positionOffset;
}
    
HUGO::Quaternion RigidBodyComponent::getRotationOffset() const
{
    return rotationOffset;
}
