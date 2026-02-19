#include "ComponentDenseTable.h"

ComponentDenseTable::ComponentDenseTable() {
  componentLists.fill(nullptr);
}

EntityID
ComponentDenseTable::removeComponent(ComponentID componentID, DenseListIndex index) {
  if(componentLists[componentID] == nullptr) return 0u;

  return componentLists[componentID]->remove(index);
}

DenseListIndex 
ComponentDenseTable::getCount(ComponentID componentID) const {
  if(componentLists[componentID] == nullptr) return 0u;
  return componentLists[componentID]->getCount();
}
