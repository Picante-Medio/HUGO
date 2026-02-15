#include "ComponentDenseTable.h"

ComponentDenseTable::ComponentDenseTable() {
    componentLists.fill(nullptr);
    activeSize.fill(0u);
}

EntityID
ComponentDenseTable::removeComponent(ComponentID componentID, DenseListIndex index) {
  return componentLists[componentID]->remove(index);
}

DenseListIndex 
ComponentDenseTable::getCount(ComponentID componentID) const {
  return componentLists[componentID]->getCount();
}
