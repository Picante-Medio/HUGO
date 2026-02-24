#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene)
: System(scene)
{

}

void PhysicsSystem::update(double dt) {
    // sync logic with calculated physics
}

void PhysicsSystem::fixedUpdate(double dt) {
    // sync physics world with logic info
}