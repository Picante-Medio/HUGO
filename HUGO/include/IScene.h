#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "ComponentDenseTable.h"
#include "Ecs.h"
#include "EntityTable.h"
#include "ComponentRegistry.h"
#include "HQuery.h"

class IScene {
protected:

	EntityTable entityTable;
  	ComponentDenseTable componentTable;

	template<typename T> 
	T &getComponent(EntityID entity) {
	ComponentID componentID = ComponentRegistry::getComponentId<T>();
		return componentTable.getComponent<T>(
			entityTable.getDenseListIndex(entity, componentID));
	}

public:
	// Entity handling
	virtual EntityID createEntity() = 0;
	virtual void removeEntity(EntityID entity) = 0;

	// Component handling
	template <typename T, typename... Ts>
	void addComponent(EntityID entity, Ts &&...args) {
		// Creating the component
		T component(entity, std::forward<Ts>(args)...);
		ComponentID id = ComponentRegistry::getComponentId<T>();

		// Adding the component
		DenseListIndex index = componentTable.addComponent(component, entity);
		entityTable.registerComponent(entity, id, index);
	}

	template <typename T>
	T& addAndGetComponent(EntityID entity) {
		// Creating the component
		T component(entity);
		ComponentID id = ComponentRegistry::getComponentId<T>();

		// Adding the component
		DenseListIndex index = componentTable.addComponent(component, entity);
		entityTable.registerComponent(entity, id, index);

		return getComponent<T>(entity);
	}

	template <typename T> 
	void removeComponent(EntityID entity) {
		// Obtaining the index of the component to delete
		ComponentID id = ComponentRegistry::getComponentId<T>();
		DenseListIndex oldIndex = entityTable.getDenseListIndex(entity, id);

		// Deleting the component
		entityTable.deRegisterComponent(entity, id);
		auto [needsUpdate, toUpdate] = componentTable.removeComponent<T>(oldIndex);

		// Updating the index of the moved component upon deletion
		if(needsUpdate) entityTable.updateDenseListIndex(toUpdate, id, oldIndex);
	}

	template <typename... Ts> 
	HUGO::Query<Ts...> query() {
		HUGO::Query<Ts...> returnedQuery;

		// Expand the parameter pack to get the affected components by ID
		std::vector<ComponentID> ids = ComponentRegistry::getAllComponentIds<Ts...>();

		// Create the bitmask to avoid hasComponent operations
		std::bitset<MAX_COMPONENTS> queryMask;
		for (ComponentID id : ids)
		queryMask.set(id);

		// Get the smallest list of possible entities
		int smallestListIndex = 0;
		DenseListIndex smallestSize = componentTable.getCount(ids[0]);
		for (size_t i = 1; i < ids.size(); ++i) {
			DenseListIndex size = componentTable.getCount(ids[i]);
			if (size < smallestSize) {
				smallestSize = size;
				smallestListIndex = i;
			}
		}

		ComponentID componentID = ids[smallestListIndex];
		std::vector<EntityID> potentialEntities = componentTable.affectedEntities(componentID);

		// Add components of the right entities to the query
		for (EntityID entity : potentialEntities)
			if (entityTable.isInQuery(entity, queryMask))
			returnedQuery.push_back(getComponent<Ts>(entity)...);

		return returnedQuery;
	}
};
