#pragma once
#include "RigidBodyComponent.h"

RigidBodyComponent::RigidBodyComponent(EntityID entity, Shape shape)
: Component(entity)
, rbShape(shape)
{}

#pragma region Setters
// ----------------------Setters----------------------
    
void RigidBodyComponent::setRestitution(float nRestitution)
{
    restitution = nRestitution;
    // Update in bullet
}

void RigidBodyComponent::setFriction(float nFriction)
{
    friction = nFriction;
    // Update in bullet
}
    
void RigidBodyComponent::setMass(float nMass)
{
    mass = nMass;
    // Update in bullet
}
    
void RigidBodyComponent::setDamping(float nDamping)
{
    damping = nDamping;
    // Update in bullet
}

void RigidBodyComponent::applyImpulse(HUGO::Vector3 nImpulse)
{
    // Update in bullet
    // linearVelocity = {bullet.linearVelocity}
}
    
void RigidBodyComponent::applyTorqueImpulse(HUGO::Vector3 nImpulse)
{
    // Update in bullet
    // angularVelocity = {bullet.angularVelocity}
}
    
void RigidBodyComponent::applyForce(HUGO::Vector3 nForce)
{
    // Update in bullet
    // linearAcceleration = {bullet.linearAcceleration}
}
    
void RigidBodyComponent::applyTorque(HUGO::Vector3 nTorque)
{
    // Update in bullet
    // angularAcceleration = {bullet.angularAcceleration}
}

void RigidBodyComponent::setPositionOffset(HUGO::Vector3 offset)
{
    positionOffset = offset;
    // Update in bullet
}
    
void RigidBodyComponent::setRotationOffset(HUGO::Quaternion offset)
{
    rotationOffset = offset;
    // Update in bullet
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