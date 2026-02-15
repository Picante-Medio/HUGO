#pragma once

#include <utility>
#include <vector>

#include "Ecs.h"
#include "ComponentDenseTable.h"
#include "EntityTable.h"
#include "System.h"

class Scene {
  EntityTable entityIndexes;
  ComponentDenseTable componentTable;
  std::vector<System> systems;

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
    T component(entity, std::forward<Ts>(args)...);
    ComponentID id = getComponentId<T>();

    entityIndexes.registerComponent(entity, id, componentTable.addComponent(component, entity));
  }

  template<typename T>
  void removeComponent(EntityID entity) {
    ComponentID id = getComponentId<T>();
    DenseListIndex oldIndex = entityIndexes.getDenseListIndex(entity, id);
    entityIndexes.deRegisterComponent(entity, id);
    EntityID toUpdate = componentTable.removeComponent<T>();
    entityIndexes.updateDenseListIndex(toUpdate, id, oldIndex);
  }
};
