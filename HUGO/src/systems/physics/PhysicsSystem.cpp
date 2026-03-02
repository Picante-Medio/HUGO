#include <iostream>

#include "PhysicsSystem.h"
#include "Scene.h"


PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene)
: System(scene)
{

}

PhysicsSystem::~PhysicsSystem() 
{

}

void PhysicsSystem::update(double dt) 
{

}

void PhysicsSystem::fixedUpdate(double dt) 
{

}
