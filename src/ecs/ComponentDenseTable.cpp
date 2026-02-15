#include "ComponentDenseTable.h"

ComponentDenseTable::ComponentDenseTable() {

}

EntityID 
ComponentDenseTable::removeComponent(ComponentID id, ComponentIndex index) {
  return componentLists[id]->remove(index);
}

ComponentIndex 
ComponentDenseTable::getCount(ComponentID id) const {
  return componentLists[id]->getCount();
}
