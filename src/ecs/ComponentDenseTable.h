#pragma once

#include <vector>
#include <array>

#include "Component.h"

class ComponentDenseTable {
  std::vector<std::array<Component, MAX_COMPONENTS>> components; // Components packed and alligned in memory
  std::vector<unsigned long int> last; // Índice del último componente válido de la lista

public:
  ComponentDenseTable();

  Component& getComponent(ComponentID componentID, unsigned int direction);
  void removeComponent(ComponentID componentID, unsigned int direction);
  unsigned int addComponent(ComponentID componentID, Component component);
};
