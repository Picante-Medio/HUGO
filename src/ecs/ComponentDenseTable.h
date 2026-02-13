#pragma once

#include <vector>
#include <array>

#include "Component.h"
#include "Ecs.h"

using ComponentIndex = unsigned long int;

class ComponentDenseTable {
  std::array<std::vector<Component>, MAX_COMPONENTS> components; // Components packed and alligned in memory
  std::array<ComponentIndex, MAX_COMPONENTS> lastAlive; // Index of the last component alive of a certain type

public:
  ComponentDenseTable();

  Component& getComponent(ComponentID componentID, unsigned int direction);
  void removeComponent(ComponentID componentID, unsigned int direction);
  ComponentIndex addComponent(ComponentID componentID, Component component);
};
