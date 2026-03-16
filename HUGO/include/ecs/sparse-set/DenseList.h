#pragma once

#include <iostream>
#include <utility>
#include <vector>

#include "EcsTypes.h"

/**
 * @brief Middleground interface for the generic dense list
 */
class IDenseList {
public:
	virtual DenseListIndex getCount() const = 0;
	virtual std::pair<bool, EntityID> remove(DenseListIndex index) = 0;
	virtual std::vector<EntityID> getAffectedEntities() const = 0;
};

/**
 * Generic dense list of components
 */
template <typename T> class DenseList : public IDenseList {
	std::vector<T> components;
	std::vector<EntityID> entityIdList;
	DenseListIndex activeComponentCount;

public:
	DenseList() : components(), activeComponentCount(0u) {}

	/**
	 * @brief stores a component in the dense list
	 * @param component the component to store
	 * @param entity the entity that the component is attached to
	 * @return the position on the dense list of the stored component
	 */
	DenseListIndex addComponent(const T &component, EntityID entity) {
		if (activeComponentCount == components.size()) {
		components.push_back(component);
		entityIdList.push_back(entity);
		}

		else {
		components[activeComponentCount] = component;
		entityIdList[activeComponentCount] = entity;
		}

		activeComponentCount++;
		return activeComponentCount - 1;
	}

	/**
	 * @return the amount of valid components stored
	 */
	inline DenseListIndex getCount() const override { return activeComponentCount; }

	/**
	 * @brief eliminates a component from the dense list keeping every active component adjacent
	 * @param index index of the component to remove
	 * @return the id of the entity which component has moved to preserve order. The new index of said component is the value of the index parameter
	 */
	std::pair<bool, EntityID> remove(DenseListIndex index) override {
		if (activeComponentCount == 0)
			throw std::runtime_error("Remove from empty DenseList");

		if (index >= activeComponentCount)
			throw std::out_of_range("DenseList::remove index out of range");

		activeComponentCount--;

		bool needUpdate = index != activeComponentCount;
		EntityID movedEntity = entityIdList[activeComponentCount];
		if (needUpdate) {
			components[index] = std::move(components[activeComponentCount]);
			entityIdList[index] = movedEntity;
			return {needUpdate, movedEntity};
		}

		else return {false, 0u};
	}

	/**
	 * @param index index where the component is stored
	 * @returns a reference to the component stored at position index
	 */
	T &getComponent(DenseListIndex index) {
		if (index >= activeComponentCount)
		throw "Out of range";

		return components[index];
	}


	std::vector<EntityID> getAffectedEntities() const override {
		//Slice of the vector with only the alive entities
		return std::vector<EntityID>(entityIdList.begin(), entityIdList.begin() + activeComponentCount);
	}
};
