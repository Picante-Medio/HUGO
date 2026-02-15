#pragma once

#include <vector>

// Entities are just numbers
using EntityID = unsigned long int;

// Identifier assigned dynamically to every component
using ComponentID = unsigned int;

// Component ID limit
constexpr ComponentID MAX_COMPONENTS = 256;

// Index on the component dense list
using DenseListIndex = unsigned long int;


/**
  * @brief auxiliar function that generates IDs dinamically every time it is called
  * @return a unique ID
*/
static ComponentID getNextID() {
  static ComponentID index = 0;
  index += 1;

  if(index >= MAX_COMPONENTS) throw "Too many components";
  return index;
}

/**
  * @brief returns a unique ID for every type that it is called with
  * @tparam T the type of the component that needs a unique ID
  * @return a unique ID
*/
template<typename T>
static ComponentID getComponentId() {
  static ComponentID index = getNextID();
  return index;
}

template<typename... Ts>
std::vector<int> getAllComponentIds() {
    return { getComponentId<Ts>()... };
}
