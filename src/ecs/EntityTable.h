#pragma once

#include <bitset>
#include <queue>
#include <vector>
#include <array>

#include "Ecs.h"

class EntityList {
  struct EntityData {
    std::vector<std::bitset<MAX_COMPONENTS>> componentMask; //Makes checking if a component is in an entity constant
    std::vector<std::array<int, MAX_COMPONENTS>> componentDirections; //Component directions in the dense table
  };

  EntityData entityData;
  std::queue<EntityID> availableEntitySpaces;

public:
  EntityList();

  // Manejo de entidades
  EntityID createEntity();
  void removeEntity(EntityID entity);

  // Manejo de componentes
  void registerComponent(EntityID entity, ComponentID componentID, ComponentID componentDirection);
  void deRegisterComponent(EntityID entity, ComponentID componentID);

  bool hasComponent(EntityID entity, ComponentID componentID) const;
  unsigned int getComponentDirection(EntityID entity, ComponentID componentID) const;

  // Used to delete all components of a dead entity
  std::vector<std::pair<ComponentID, unsigned int>> getAllComponentDirections(EntityID entity);
};
