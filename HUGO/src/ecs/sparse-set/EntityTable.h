#pragma once

#include "IEntityTable.h"

/**
 * @brief object that stores dense list indexes for every valid component of
 * every alive entity
 */
class EntityTable: public IEntityTable {
	// Mask used to check if a component is in an entity
	std::vector<std::bitset<MAX_COMPONENTS>> componentMask;

	// Component directions in the dense table 
	std::vector<std::array<DenseListIndex, MAX_COMPONENTS>> componentDirections;

	// Positions of entities that have been removed and can be occupied again
	std::queue<EntityID> availableEntitySpaces;

public:
	EntityTable();

	/**
	 * @brief creates an entity
	 * @return the ID of the new entity
	 */
	EntityID createEntity() override;

	/** 
	 * @brief frees the index of a no longer in use entity
	 */
	void removeEntity(EntityID entity) override;

	/**
	 * @brief sets the dense list index of a component of known ID attached to a given entity. Also sets the "attached" flag to true
	 * @param entity the entity that the component needs to be attached to
	 * @param componentID the unique ID of the type of the component
	 * @param index the position in the dense list of the component
	 */
	void registerComponent(EntityID entity, ComponentID componentID, DenseListIndex index) override;

	/**
	 * @brief removes the "attached" flag of a component of known ID attached to a given entity
	 * @param entity the entity that the component needs to be attached to
	 * @param componentID the unique ID of the type of the component
	 */
	void deRegisterComponent(EntityID entity, ComponentID componentID) override;


	/**
	 * @brief sets the dense list index of a component of known ID attached to a given entity. Does not set the "attached" flag to true
	 * @param entity the entity that the component is attached to
	 * @param componentID the unique ID of the type of the component
	 * @param index the position in the dense list of the component
	 */
	void updateDenseListIndex(EntityID entity, ComponentID componentID, DenseListIndex index) override;

	/**
	 * @brief checks if an entity has a certain type of component attached to it
	 * @param entity the entity that the component is or is not attached to
	 * @param componentID the unique ID of the type of the component
	 * @return true if entity has the component, false otherwise
	 */
	bool hasComponent(EntityID entity, ComponentID componentID) const override;

	/**
	 * @brief checks if an entity has a group of components specified by a mask
	 * @param entity the entity that might be affected by the query
	 * @param queryMask the bitmask that specifies which components the entity must have to be part of the query
	 * @return true if the entity is affected by the query, false otherwise
	 */
	bool isInQuery(EntityID entity, std::bitset<MAX_COMPONENTS> queryMask) override;

	/** 
	 * @param entity the entity that the component is attached to
	 * @param componentID the unique ID of the type of the component
	 * @return the index in the dense list of the component
	 */
	DenseListIndex getDenseListIndex(EntityID entity, ComponentID componentID) const override;

	/** 
	 * Used to delete every component of an invalid entity upon deletion
	 * @return a list of ComponentID and indexes of said components in the dense table
	 */
	std::vector<std::pair<ComponentID, DenseListIndex>> getAllDenseListIndexes(EntityID entity) override;
	
		/** 
	 * Used to read all entities components for debugging
	 * @return the vector of bitsets
	 */
	std::vector<std::bitset<MAX_COMPONENTS>> getComponentMask() override;
};
