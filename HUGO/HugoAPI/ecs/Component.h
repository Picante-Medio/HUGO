# pragma once

#include "EcsTypes.h"

// Components are just data
struct Component {
  EntityID entity;

  Component(EntityID entity) : entity(entity) {}
};
