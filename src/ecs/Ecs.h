#pragma once

// Entities are just numbers
using EntityID = unsigned long int;

// Identifier assigned dynamically to every component
using ComponentID = unsigned int;

// Component ID limit
constexpr ComponentID MAX_COMPONENTS = 256;

// Index on the component dense list
using DenseListIndex = unsigned long int;


// Static methods to create the IDs dinamically
static ComponentID getNextID() {
  static ComponentID index = 0;
  index += 1;

  if(index >= MAX_COMPONENTS) throw "Too many components";
  return index;
}

// Returns the same ID on everry call with a certain type. It is only meant to be used with component types
template<typename T>
static ComponentID getComponentId() {
  static ComponentID index = getNextID();
  return index;
}
