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

float RigidBodyComponent::getRestitution()
{
    return restitution;
}
    
float RigidBodyComponent::getFriction()
{
    return friction;
}
    
float RigidBodyComponent::getMass()
{
    return mass;
}
    
float RigidBodyComponent::getDamping()
{
    return damping;
}

HUGO::Vector3 RigidBodyComponent::getLinearVelocity()
{
    return linearVelocity;
}
    
HUGO::Vector3 RigidBodyComponent::getAngularVelocity()
{
    return angularVelocity;
}
    
HUGO::Vector3 RigidBodyComponent::getLinearAcceleration()
{
    return linearAcceleration;
}
    
HUGO::Vector3 RigidBodyComponent::getAngularAcceleration()
{
    return angularAcceleration;
}

HUGO::Vector3 RigidBodyComponent::getPositionOffset()
{
    return positionOffset;
}
    
HUGO::Quaternion RigidBodyComponent::getRotationOffset()
{
    return rotationOffset;
}