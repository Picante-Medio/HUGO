#include "EntityTable.h"
#include "EcsTypes.h"
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
  componentMask[entity].set(componentID, true);
  componentDirections[entity][componentID] = index;
}

void 
EntityTable::deRegisterComponent(EntityID entity, ComponentID componentID) {
  componentMask[entity].set(componentID, false);
}

void 
EntityTable::updateDenseListIndex(EntityID entity, ComponentID componentID, DenseListIndex index) {
  componentDirections[entity][componentID] = index;
}

bool 
EntityTable::hasComponent(EntityID entity, ComponentID componentID) const {
  return componentMask[entity].test(componentID);
}


bool 
EntityTable::isInQuery(EntityID entity, std::bitset<MAX_COMPONENTS> queryMask) {
  return (componentMask[entity] & queryMask) == queryMask;
}

DenseListIndex 
EntityTable::getDenseListIndex(EntityID entity, ComponentID componentID) const {
  if(hasComponent(entity, componentID)) return componentDirections[entity][componentID];
  else throw "Esto nunca deberia pasar";
}

std::vector<std::pair<ComponentID, DenseListIndex>> 
EntityTable::getAllDenseListIndexes(EntityID entity) {
  std::vector<std::pair<ComponentID, DenseListIndex>> list;

  for(ComponentID id = 0; id < MAX_COMPONENTS; ++id)
    if(componentMask[entity].test(id))
      list.push_back({id, componentDirections[entity][id]});

  return list;
}

std::vector<std::bitset<MAX_COMPONENTS>> 
EntityTable::getComponentMask() {
  return componentMask;
}