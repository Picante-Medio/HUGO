#pragma once

#include "Ecs.h"
#include "ComponentDenseTable.h"
#include "EntityTable.h"

class Scene {
  EntityList entityDirections;
  ComponentDenseTable components;

public:
  Scene();

  // Logic update
  void update(double dt);

  // Physics update
  void fixedUpdate(double dt);

  // Entity handling
  EntityID createEntity();
  void removeEntity(EntityID entity);

  // Component handling
  template<typename T, typename ...Ts>
  void addComponent(EntityID entity, Ts &&... args) {

  }

  template<typename T>
  void removeComponent(EntityID entity) {

  }
};
