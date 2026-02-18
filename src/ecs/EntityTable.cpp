#include "EntityTable.h"
#include "Ecs.h"
#include <vector>

EntityTable::EntityTable() 
{}

EntityID EntityTable::createEntity() {
  EntityID newID;
  if (!availableEntitySpaces.empty()) {
    newID = availableEntitySpaces.front();
    availableEntitySpaces.pop();
  }
  else {
    componentDirections.push_back(std::array<DenseListIndex, MAX_COMPONENTS>());
    componentMask.push_back(std::bitset<MAX_COMPONENTS>());
    newID = componentMask.size() - 1;
  }

  return newID;
}

void EntityTable::removeEntity(EntityID entity) {
  componentMask[entity].reset();
  availableEntitySpaces.push(entity);
}

void 
EntityTable::registerComponent(EntityID entity, ComponentID componentID, DenseListIndex index) {
  componentMask[entity][componentID] = 1;
  componentDirections[entity][componentID] = index;
}

void 
EntityTable::deRegisterComponent(EntityID entity, ComponentID componentID) {
  componentMask[entity][componentID] = 0;
}

void 
EntityTable::updateDenseListIndex(EntityID entity, ComponentID componentID, DenseListIndex index) {
  componentDirections[entity][componentID] = index;
}

bool 
EntityTable::hasComponent(EntityID entity, ComponentID componentID) const {
  return componentMask[entity][componentID];
}


bool 
EntityTable::isInQuery(EntityID entity, std::bitset<MAX_COMPONENTS> queryMask) {
  return (componentMask[entity] & queryMask) == queryMask;
}

DenseListIndex 
EntityTable::getDenseListIndex(EntityID entity, ComponentID componentID) const {
  return componentDirections[entity][componentID];
}

std::vector<std::pair<ComponentID, DenseListIndex>> 
EntityTable::getAllDenseListIndexes(EntityID entity) {
  std::vector<std::pair<ComponentID, DenseListIndex>> list;

  for(ComponentID id = 0; id < MAX_COMPONENTS; ++id) {
    if(componentMask[entity][id]) continue;

    list.push_back({id, componentDirections[entity][id]});
  }

  return list;
}
