#include "RigidBodyComponent.h"

RigidBodyComponent::RigidBodyComponent(EntityID entity, Shape shape, float mass)
: Component(entity)
, rbShape(shape)
, flags(0)
, mass(mass)
, restitution(0.f) // default bullet value
, friction(.5f) // default bullet value
, linearDamping(0.f) // default bullet value
, angularDamping(0.f) // default bullet value
, positionOffset(HUGO::Vector3()) // default bullet value
, rotationOffset(HUGO::Quaternion()) // default bullet value
, linearVelocity(HUGO::Vector3()) // default bullet value
, angularVelocity(HUGO::Vector3()) // default bullet value
, active(true)
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
    
void RigidBodyComponent::setLinearDamping(float nDamping)
{
    linearDamping = nDamping;
    flags = flags | UpdateFlags::DAMPING_UPDATED;
}

void RigidBodyComponent::setAngularDamping(float nDamping)
{
    angularDamping = nDamping;
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
    flags = flags | UpdateFlags::TRANSFORM_OFFSET_UPDATED;
}
    
void RigidBodyComponent::setRotationOffset(HUGO::Quaternion offset)
{
    rotationOffset = offset;
    flags = flags | UpdateFlags::TRANSFORM_OFFSET_UPDATED;
}

void RigidBodyComponent::setActive(bool isActive)
{
    active = isActive;
    flags = flags | UpdateFlags::ACTIVE_UPDATED;
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
    
float RigidBodyComponent::getLinearDamping() const
{
    return linearDamping;
}

float RigidBodyComponent::getAngularDamping() const
{
    return angularDamping;
}

HUGO::Vector3 RigidBodyComponent::getLinearVelocity() const
{
    return linearVelocity;
}
    
HUGO::Vector3 RigidBodyComponent::getAngularVelocity() const
{
    return angularVelocity;
}

HUGO::Vector3 RigidBodyComponent::getPositionOffset() const
{
    return positionOffset;
}
    
HUGO::Quaternion RigidBodyComponent::getRotationOffset() const
{
    return rotationOffset;
}

bool RigidBodyComponent::getActive() const
{
    return active;
}