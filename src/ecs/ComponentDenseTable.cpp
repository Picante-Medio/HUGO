#include "ComponentDenseTable.h"
#include "Ecs.h"

ComponentDenseTable::ComponentDenseTable()
{
  lastAlive.fill(0u);
}

Component& 
ComponentDenseTable::getComponent(ComponentID componentID, unsigned int direction) {
  return components[componentID][direction];
}

void
ComponentDenseTable::removeComponent(ComponentID componentID, unsigned int direction) {
  Component temp = components[componentID][lastAlive[componentID]];
  components[componentID][lastAlive[componentID]] = components[componentID][direction];
  components[componentID][direction] = temp;
  lastAlive[componentID]--;
}

ComponentIndex
ComponentDenseTable::addComponent(ComponentID componentID, Component component) {
  lastAlive[componentID]++;

  if(lastAlive[componentID] == components[componentID].size()) 
    components[componentID].push_back(component);

  else
    components[componentID][lastAlive[componentID]] = component;

  return lastAlive[componentID];
}
