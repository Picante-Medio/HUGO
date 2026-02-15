#pragma once

#include <bitset>
#include <queue>
#include <vector>
#include <array>

#include "Ecs.h"

class EntityTable {
  std::vector<std::bitset<MAX_COMPONENTS>> componentMask; //Makes checking if a component is in an entity constant
  std::vector<std::array<int, MAX_COMPONENTS>> componentDirections; //Component directions in the dense table

  std::queue<EntityID> availableEntitySpaces;

public:
  EntityTable();

  // Manejo de entidades
  EntityID createEntity();
  void removeEntity(EntityID entity);

  // Manejo de componentes
  void registerComponent(EntityID entity, ComponentID componentID, ComponentIndex index);
  void deRegisterComponent(EntityID entity, ComponentID componentID);
  void updateComponentIndex(EntityID entity, ComponentID componentID, ComponentIndex index);

  bool hasComponent(EntityID entity, ComponentID componentID) const;
  unsigned int getComponentIndex(EntityID entity, ComponentID componentID) const;

  // Used to delete all components of a dead entity
  std::vector<std::pair<ComponentID, ComponentIndex>> getAllComponentIndexes(EntityID entity);
};

