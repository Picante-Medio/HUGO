#pragma once

// Entities are just numbers
using EntityID = unsigned long int;

constexpr EntityID INVALID_ENTITY_ID = std::numeric_limits<EntityID>::max();

// Identifier assigned dynamically to every component
using ComponentID = unsigned int;

// Component ID limit
constexpr ComponentID MAX_COMPONENTS = 255;

// Index on the component dense list
using DenseListIndex = unsigned long int;
