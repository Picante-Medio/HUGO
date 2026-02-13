#include "EntityTable.h"
#include <vector>

EntityList::EntityList() 
{}

EntityID EntityList::createEntity() {
  EntityID newID;
  if (!availableEntitySpaces.empty()) {
    newID = availableEntitySpaces.front();
    availableEntitySpaces.pop();
  }
  else {
    entityData.componentDirections.push_back(std::array<int, MAX_COMPONENTS>());
    entityData.componentMask.push_back(std::bitset<MAX_COMPONENTS>());
    newID = entityData.componentMask.size() - 1;
  }

  return newID;
}

void EntityList::removeEntity(EntityID entity) {
  entityData.componentMask[entity].reset();
  availableEntitySpaces.push(entity);
}

void 
EntityList::registerComponent(EntityID entity, ComponentID componentID, ComponentID componentDirection) {
  entityData.componentMask[entity][componentID] = 1;
  entityData.componentDirections[entity][componentID] = componentDirection;
}

void 
EntityList::deRegisterComponent(EntityID entity, ComponentID componentID) {
  entityData.componentMask[entity][componentID] = 0;
}

bool 
EntityList::hasComponent(EntityID entity, ComponentID componentID) const {
  return entityData.componentMask[entity][componentID];
}

unsigned int 
EntityList::getComponentDirection(EntityID entity, ComponentID componentID) const {
  return entityData.componentDirections[entity][componentID];
}
