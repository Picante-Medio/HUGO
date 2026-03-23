#pragma once

#include <memory>
#include <vector>
#include <array>

#include "EcsTypes.h"
#include "DenseList.h"

#include "ComponentRegistry.h"
#include "ComponentEvent.h"

/// @brief Helper structure to group the events of each component
struct ComponentModificationEvents {
	// Events must be shared ptr to allow casting
	std::shared_ptr<IComponentEvent> onAdd = nullptr;
	std::shared_ptr<IComponentEvent> onUpdate = nullptr;
	std::shared_ptr<IComponentEvent> onRemove = nullptr;
};

/**
  * @brief a list of DenseList objects, each DenseList of a different type of component
  */
class IComponentDenseTable {
protected:
	std::array<std::shared_ptr<IDenseList>, MAX_COMPONENTS> componentLists; // Components packed and alligned in memory
	std::array<std::unique_ptr<ComponentModificationEvents>, MAX_COMPONENTS> eventsList; // Events associated to each component

	/**
	 * @brief auxiliar function that creates a DenseList of the specified type if it does not exist yet
	 * @tparam the type of component that the casted list stores
	 */
	template<typename T>
	void ensureListExists() {
		ComponentID componentID = ComponentRegistry::getComponentId<T>();
		if(componentLists[componentID] != nullptr) return;
		componentLists[componentID] = std::make_shared<DenseList<T>>();

		// Create events
		eventsList[componentID] = std::make_unique<ComponentModificationEvents>();
		eventsList[componentID]->onAdd = std::make_shared<ComponentEvent<T>>();
		eventsList[componentID]->onUpdate = std::make_shared<ComponentEvent<T>>();
		eventsList[componentID]->onRemove = std::make_shared<ComponentEvent<T>>();
	}

	/**
	 * @brief auxiliar function that casts an IDenseList to a DenseList of a certain type
	 * @tparam the type of component that the casted list stores
	 * @return a pointer to the specified type of dense list
	 */
	template<typename T>
	std::shared_ptr<DenseList<T>> getComponentList() {
		ensureListExists<T>();
		ComponentID cmpID = ComponentRegistry::getComponentId<T>();
		return std::static_pointer_cast<DenseList<T>>(componentLists[cmpID]);
	}

public:

	virtual ~IComponentDenseTable() = default;

	/**
	 * @brief calls removeComponent on the DenseList that stores components of id componentID
	 * @param componentID id of the component to be removed
	 * @param index index in the denselist of the component to be removed
	 * @return the id of the entity which component has moved to preserve order. The new index of said component is the value of the index parameter
	 */
	virtual std::pair<bool, EntityID> removeComponent(ComponentID componentID, DenseListIndex index) = 0;
	virtual DenseListIndex getCount(ComponentID componentID) const = 0;

	/**
	 * @brief calls getComponent on the DenseList that stores components of type T
	 * @tparam the type of the returned component
	 * @param index the index of the returned component in the DenseList
	 * @return a reference to the stored component
	 */
	template<typename T>
	T& getComponent(DenseListIndex index) {
		return getComponentList<T>()->getComponent(index);
	}

	/**
	 * @brief calls addComponent on the DenseList that stores components of type T
	 * @tparam the type of the added component
	 * @param component the component to add
	 * @param entity the ID of the entity that the component is attached to
	 * @return the position on the dense list of the stored component
	 */
	template<typename T>
	DenseListIndex addComponent(const T& component, EntityID entity) {
		return getComponentList<T>()->addComponent(component, entity);
	}

  
	/**
	 * @brief calls removeComponent on the DenseList that stores components of type T
	 * @tparam the type of the removed component
	 * @param index the index in the DenseList of the component to remove
	 * @return the id of the entity which component has moved to preserve order. The new index of said component is the value of the index parameter
	 */
	template<typename T>
	std::pair<bool, EntityID> removeComponent(DenseListIndex index) {
		return getComponentList<T>()->remove(index);
	};

	/// @brief Returns a remporal reference to the addEvent of T
	/// @tparam T Component
	/// @return 
	template<typename T>
	std::shared_ptr<ComponentEvent<T>> getAddEvent() {
		ensureListExists<T>();
		ComponentID cmpID = ComponentRegistry::getComponentId<T>();
		return std::static_pointer_cast<ComponentEvent<T>>(eventsList[cmpID]->onAdd);
	}

	/// @brief Returns a remporal reference to the updateEvent of T
	/// @tparam T Component
	/// @return 
	template<typename T>
	std::shared_ptr<ComponentEvent<T>> getUpdateEvent() {
		ensureListExists<T>();
		ComponentID cmpID = ComponentRegistry::getComponentId<T>();
		return std::static_pointer_cast<ComponentEvent<T>>(eventsList[cmpID]->onUpdate);
	}

	/// @brief Returns a remporal reference to de removeEvent of T
	/// @tparam T Component
	/// @return 
	template<typename T>
	std::shared_ptr<ComponentEvent<T>> getRemoveEvent() {
		ensureListExists<T>();
		ComponentID cmpID = ComponentRegistry::getComponentId<T>();
		return std::static_pointer_cast<ComponentEvent<T>>(eventsList[cmpID]->onRemove);
	}

	/**
	 * @brief calls getCount on the DenseList that stores components of type T
	 * @tparam the type of the removed component
	 * @return the number of valid components stored in the DenseList
	 */
	template<typename T>
	DenseListIndex getCount() const {
		return getCount(ComponentRegistry::getComponentId<T>(ComponentRegistry::getComponentId<T>()));
	}

	virtual std::vector<EntityID> affectedEntities(ComponentID componentID) = 0;
};
