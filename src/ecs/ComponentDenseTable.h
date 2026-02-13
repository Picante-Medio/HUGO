#pragma once

#include <vector>
#include <array>

#include "Component.h"
#include "Ecs.h"

class ComponentDenseTable {
  std::array<std::vector<Component>, MAX_COMPONENTS> components; // Components packed and alligned in memory
  std::array<unsigned long int, MAX_COMPONENTS> last; // Índice del último componente válido de la lista

public:
  ComponentDenseTable();

  Component& getComponent(ComponentID componentID, unsigned int direction);
  void removeComponent(ComponentID componentID, unsigned int direction);
  unsigned int addComponent(ComponentID componentID, Component component);
};
