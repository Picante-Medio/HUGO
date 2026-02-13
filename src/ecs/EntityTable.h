#pragma once

#include <bitset>
#include <queue>
#include <vector>
#include <array>

// Entities are just numbers
using EntityID = unsigned long int;
using ComponentID = unsigned int;
constexpr unsigned int MAX_COMPONENTS = 256;

class EntityList {
  struct EntityData {
    std::vector<std::bitset<MAX_COMPONENTS>> componentMask; //Makes checking if a component is in an entity constant
    std::vector<std::array<int, MAX_COMPONENTS>> componentDirections; //Component direction table
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
  int getComponentDirection(EntityID entity, ComponentID componentID) const;
};
