#pragma once

// Entities are just numbers
using EntityID = unsigned long int;

// Types and values
using ComponentID = unsigned int;
constexpr ComponentID MAX_COMPONENTS = 256;

using ComponentIndex = unsigned long int;

// ID casting
static ComponentID getNextID() {
  static ComponentID index = 0;
  return index++;
}

template<typename T>
static ComponentID getComponentId() {
  static ComponentID index = getNextID();
  return index;
}
