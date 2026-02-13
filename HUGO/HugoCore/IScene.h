#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ComponentDenseTable.h"
#include "Ecs.h"
#include "EntityTable.h"
#include "HQuery.h"
#include "System.h"
#include "HugoTypes.h"
#include "BulletSimulation.h"

#include <unordered_map>

#include "HugoError.h"

class ComponentRegistry {
	ComponentID currentId = 0;
	std::unordered_map<std::string, ComponentID> registry = std::unordered_map<std::string, ComponentID>();
public:

	ErrorOrType<ComponentID> getRegistryComponentID(std::string componentName) {
		if (componentName.empty()) {
			StringError err;
			err.errorMessage = "El componente no tiene nombre";
			return ErrorOrType<ComponentID>(err);
		}
		else {
			if (registry.count(componentName)) {
				return ErrorOrType<ComponentID>(registry[componentName]);
			}
			else {
				if(currentId == MAX_COMPONENTS) {
					StringError err;
					err.errorMessage = "Alcanzado límite de componentes";
					return ErrorOrType<ComponentID>(err);
				}
				registry[componentName] = currentId++;
				return ErrorOrType<ComponentID>(registry[componentName]);
			}
		}
	}
};

class IScene {
private:
	const ComponentRegistry registry;
public:
	// Entity handling
	EntityID createEntity();
	void removeEntity(EntityID entity);

	template<typename T>
	ComponentID getComponentId() {
	static ComponentID index = getNextID();
	return index;
	}

	// Component handling
	template <typename T, typename... Ts>
	void addComponent(EntityID entity, Ts &&...args) {
		// Creating the component
		T component(entity, std::forward<Ts>(args)...);
		ComponentID id = getComponentId<T>();

		// Adding the component
		DenseListIndex index = componentTable.addComponent(component, entity);
		entityTable.registerComponent(entity, id, index);
	}

	template <typename T> 
	void removeComponent(EntityID entity) {
		// Obtaining the index of the component to delete
		ComponentID id = getComponentId<T>();
		DenseListIndex oldIndex = entityTable.getDenseListIndex(entity, id);

		// Deleting the component
		entityTable.deRegisterComponent(entity, id);
		auto [needsUpdate, toUpdate] = componentTable.removeComponent<T>(oldIndex);

		// Updating the index of the moved component upon deletion
		if(needsUpdate) entityTable.updateDenseListIndex(toUpdate, id, oldIndex);
	}

	template <typename... Ts> 
	HUGO::Query<Ts...> query() {
		Query<Ts...> returnedQuery;

		// Expand the parameter pack to get the affected components by ID
		std::vector<ComponentID> ids = getAllComponentIds<Ts...>();

		// Create the bitmask to avoid hasComponent operations
		std::bitset<MAX_COMPONENTS> queryMask;
		for (ComponentID id : ids)
		queryMask.set(id);

		// Get the smallest list of possible entities
		int smallestListIndex = 0;
		DenseListIndex smallestSize = componentTable.getCount(ids[0]);
		for (int i = 1; i < ids.size(); ++i){
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