#include "EntityTable.h"
#include "Ecs.h"
#include <utility>
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
    componentDirections.push_back(std::array<int, MAX_COMPONENTS>());
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
EntityTable::registerComponent(EntityID entity, ComponentID componentID, ComponentIndex index) {
  componentMask[entity][componentID] = 1;
  componentDirections[entity][componentID] = index;
}

void 
EntityTable::deRegisterComponent(EntityID entity, ComponentID componentID) {
  componentMask[entity][componentID] = 0;
}

void 
EntityTable::updateComponentIndex(EntityID entity, ComponentID componentID) {

}

bool 
EntityTable::hasComponent(EntityID entity, ComponentID componentID) const {
  return componentMask[entity][componentID];
}

unsigned int 
EntityTable::getComponentIndex(EntityID entity, ComponentID componentID) const {
  return componentDirections[entity][componentID];
}

std::vector<std::pair<ComponentID, ComponentIndex>> 
EntityTable::getAllComponentIndexes(EntityID entity) {
  std::vector<std::pair<ComponentID, ComponentIndex>> list;

  for(ComponentID id = 0; id < MAX_COMPONENTS; ++id) {
    if(componentMask[entity][id]) continue;

    list.push_back({id, componentDirections[entity][id]});
  }

  return list;
}
