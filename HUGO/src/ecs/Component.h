# pragma once

#include "Ecs.h"

// Components are just data
struct Component {
  EntityID entity;

  Component(EntityID entity) : entity(entity) {}
};
