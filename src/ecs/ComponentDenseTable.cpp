#include "ComponentDenseTable.h"

ComponentDenseTable::ComponentDenseTable()
{}

Component& 
ComponentDenseTable::getComponent(ComponentID componentID, unsigned int direction) {
  return components[componentID][direction];
}

void 
ComponentDenseTable::removeComponent(ComponentID componentID, unsigned int direction) {
  Component temp = components[componentID][last[componentID]];
  components[componentID][last[componentID]] = components[componentID][direction];
  components[componentID][direction] = temp;
  last[componentID]--;
}

unsigned int
ComponentDenseTable::addComponent(ComponentID componentID, Component component) {
  last[componentID]++;
  components[componentID][last[componentID]] = component;
  return last[componentID];
}
