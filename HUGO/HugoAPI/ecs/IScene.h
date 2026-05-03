#pragma once

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include "HugoTypes.h"
#include "HugoError.h"
#include "IEntityTable.h"
#include "IComponentDenseTable.h"
#include "EcsTypes.h"
#include "ComponentRegistry.h"
#include "HugoQuery.h"
#include "Messenger.h"
#include "IInput.h"

namespace HUGO {

class IScene {
protected:

	std::unique_ptr<IEntityTable> entityTable;
  	std::unique_ptr<IComponentDenseTable> componentTable;
	Messenger messenger;

public:
	// Entity handling
	virtual EntityID createEntity() = 0;
	virtual void removeEntity(EntityID entity) = 0;

	template<typename T> 
	T &getComponent(EntityID entity) {
		ComponentID componentID = HUGO::ComponentRegistry::getComponentId<T>();
		return componentTable->getComponent<T>(
			entityTable->getDenseListIndex(entity, componentID));
	}

	template<typename T> 
	const T& getComponentConst(EntityID entity) {
		ComponentID componentID = HUGO::ComponentRegistry::getComponentId<T>();
		return componentTable->getComponent<T>(entityTable->getDenseListIndex(entity, componentID));
	}

	template<typename T, typename Func>
	void modifyComponent(EntityID entity, Func&& modifier) {

		static_assert(std::is_invocable_r_v<void, Func, T&>, "modifier() must have void(T&) signature");

		T& cmp = getComponent<T>(entity);
		std::forward<Func>(modifier)(cmp);
		componentTable->getUpdateEvent<T>()->dispatch(cmp);
	}

    template<typename T>
    bool hasComponent(EntityID entity) const {
        return entityTable->hasComponent(entity, HUGO::ComponentRegistry::getComponentId<T>());
    }

	/// @brief Adds the component with default values
	/// @tparam T Component
	/// @param entity the entity the component is part of
	template<typename T>
	void addComponent(EntityID entity) {
		// Creating the component
		T component(entity);
		ComponentID id = HUGO::ComponentRegistry::getComponentId<T>();

		// Adding the component
		DenseListIndex index = componentTable->addComponent(component, entity);
		entityTable->registerComponent(entity, id, index);

		// Dispatches addEvent
		componentTable->getAddEvent<T>()->dispatch(component);
	}

	/// @brief Adds the component and initilizes it via initializer
	/// @tparam T Component
	/// @tparam Func Initializer function
	/// @param entity the entity the component is part of
	/// @param initializer callable that is supposed to initialize the component
	template<typename T, typename Func>
	void addComponent(EntityID entity, Func&& initializer) {
		
		static_assert(std::is_invocable_r_v<void, Func, T&>, "initializer() must have void(T&) signature");

		// Creating the component
		T component(entity);
		// The component is initialized with the initializer callable
		std::forward<Func>(initializer)(component);

		ComponentID id = HUGO::ComponentRegistry::getComponentId<T>();

		// Adding the component
		DenseListIndex index = componentTable->addComponent(component, entity);
		entityTable->registerComponent(entity, id, index);

		// Dispatches addEvent
		componentTable->getAddEvent<T>()->dispatch(component);
	}

	template <typename T> 
	void removeComponent(EntityID entity) {
		// Obtaining the index of the component to delete
		ComponentID id = HUGO::ComponentRegistry::getComponentId<T>();
		DenseListIndex oldIndex = entityTable->getDenseListIndex(entity, id);

		// Calls removal event before removing
		componentTable->getRemoveEvent<T>()->dispatch(componentTable->getComponent<T>(oldIndex));

		// Deleting the component
		entityTable->deRegisterComponent(entity, id);
		auto [needsUpdate, toUpdate] = componentTable->removeComponent<T>(oldIndex);

		// Updating the index of the moved component upon deletion
		if(needsUpdate) entityTable->updateDenseListIndex(toUpdate, id, oldIndex);
	}

	template <typename T>
	ComponentEventTemporalReference<T> onAddComponent() {
		return ComponentEventTemporalReference<T>(componentTable->getAddEvent<T>());
	}

	template <typename T>
	ComponentEventTemporalReference<T> onUpdateComponent() {
		return ComponentEventTemporalReference<T>(componentTable->getUpdateEvent<T>());
	}

	template <typename T>
	ComponentEventTemporalReference<T> onRemoveComponent() {
		return ComponentEventTemporalReference<T>(componentTable->getRemoveEvent<T>());
	}

	template <typename... Ts> 
	HUGO::Query<Ts...> query() {
		HUGO::Query<Ts...> returnedQuery(componentTable->getUpdateEvent<Ts>()...);

		// Expand the parameter pack to get the affected components by ID
		std::vector<ComponentID> ids = HUGO::ComponentRegistry::getAllComponentIds<Ts...>();

		// Create the bitmask to avoid hasComponent operations
		std::bitset<MAX_COMPONENTS> queryMask;
		for (ComponentID id : ids)
		queryMask.set(id);

		// Get the smallest list of possible entities
		size_t smallestListIndex = 0;
		DenseListIndex smallestSize = componentTable->getCount(ids[0]);
		for (size_t i = 1; i < ids.size(); ++i) {
			DenseListIndex size = componentTable->getCount(ids[i]);
			if (size < smallestSize) {
				smallestSize = size;
				smallestListIndex = i;
			}
		}

		ComponentID componentID = ids[smallestListIndex];
		std::vector<EntityID> potentialEntities = componentTable->affectedEntities(componentID);

		// Add components of the right entities to the query
		for (EntityID entity : potentialEntities)
			if (entityTable->isInQuery(entity, queryMask))
				returnedQuery.push_back(getComponent<Ts>(entity)...);

		
				
		return returnedQuery;
	}

    virtual void log(HUGO::String message) = 0; 
    virtual void log(std::shared_ptr<Error>) = 0;

    virtual HUGO::ErrorOrType<HUGO::String> getNextError() = 0;
    virtual HUGO::ErrorOrType<HUGO::String> getNextLog() = 0;


	// Message handling

	// Subscribe a callback to a message channel
    template <typename T>
    void subscribe(std::function<void(std::shared_ptr<Message>)> callback) {
        messenger.subscribe<T>(callback);
    }

	// Allow a system to send a message
	template <typename TMessage>
    void sendMessage(TMessage msg) {
        messenger.enqueue(msg);
    }

	// Send all enqueued messages
	virtual void dispatchMessages() = 0;

	// Input handling
	virtual std::shared_ptr<IInput> getInput() = 0;

    /** Scene changing
     * ERROR POSSIBLE: If the scene to change does not exist, logs an error to debug console
     */
    virtual void changeScene(const HUGO::String& sceneName) = 0;
	virtual void changeAndDestroyPreviousScene(const HUGO::String& sceneName) = 0;

	virtual HUGO::ErrorOrType<EntityID> instantiatePrefab(const HUGO::String& prefabName) =0;
};

}
