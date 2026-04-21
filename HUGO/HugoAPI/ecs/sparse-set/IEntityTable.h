#pragma once

#include <array>
#include <bitset>
#include <queue>
#include <vector>

#include "EcsTypes.h"

/**
 * @brief interface of object that stores dense list indexes for every valid component of
 * every alive entity
 */
class IEntityTable {
public:
	// Virtual destructor so the implementation of EntityTable gets properly disposed of
	virtual ~IEntityTable() = default;

	/**
	 * @brief creates an entity
	 * @return the ID of the new entity
	 */
	virtual EntityID createEntity() = 0;

	/** 
	 * @brief frees the index of a no longer in use entity
	 */
	virtual void removeEntity(EntityID entity) = 0;

	/**
	 * @brief sets the dense list index of a component of known ID attached to a given entity. Also sets the "attached" flag to true
	 * @param entity the entity that the component needs to be attached to
	 * @param componentID the unique ID of the type of the component
	 * @param index the position in the dense list of the component
	 */
	virtual void registerComponent(EntityID entity, ComponentID componentID, DenseListIndex index) = 0;

	/**
	 * @brief removes the "attached" flag of a component of known ID attached to a given entity
	 * @param entity the entity that the component needs to be attached to
	 * @param componentID the unique ID of the type of the component
	 */
	virtual void deRegisterComponent(EntityID entity, ComponentID componentID) = 0;


	/**
	 * @brief sets the dense list index of a component of known ID attached to a given entity. Does not set the "attached" flag to true
	 * @param entity the entity that the component is attached to
	 * @param componentID the unique ID of the type of the component
	 * @param index the position in the dense list of the component
	 */
	virtual void updateDenseListIndex(EntityID entity, ComponentID componentID, DenseListIndex index) = 0;

	/**
	 * @brief checks if an entity has a certain type of component attached to it
	 * @param entity the entity that the component is or is not attached to
	 * @param componentID the unique ID of the type of the component
	 * @return true if entity has the component, false otherwise
	 */
	virtual bool hasComponent(EntityID entity, ComponentID componentID) const = 0;

	/**
	 * @brief checks if an entity has a group of components specified by a mask
	 * @param entity the entity that might be affected by the query
	 * @param queryMask the bitmask that specifies which components the entity must have to be part of the query
	 * @return true if the entity is affected by the query, false otherwise
	 */
	virtual bool isInQuery(EntityID entity, std::bitset<MAX_COMPONENTS> queryMask) = 0;

	/** 
	 * @param entity the entity that the component is attached to
	 * @param componentID the unique ID of the type of the component
	 * @return the index in the dense list of the component
	 */
	virtual DenseListIndex getDenseListIndex(EntityID entity, ComponentID componentID) const = 0;
	
	/** 
	 * Used to delete every component of an invalid entity upon deletion
	 * @return a list of ComponentID and indexes of said components in the dense table
	 */
	virtual std::vector<std::pair<ComponentID, DenseListIndex>> getAllDenseListIndexes(EntityID entity) = 0;

	/** 
	 * Used to read all entities components for debugging
	 * @return the vector of bitsets
	 */
	virtual std::vector<std::bitset<MAX_COMPONENTS>> getComponentMask() = 0;
};
