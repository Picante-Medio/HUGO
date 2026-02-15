#pragma once

#include <vector>

#include "Component.h"
#include "Ecs.h"

/**
 * @brief Middleground interface for the generic dense list
 */
class IDenseList {
public:
  virtual DenseListIndex getCount() const = 0;
  virtual EntityID remove(DenseListIndex index) = 0;
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
  EntityID remove(DenseListIndex index) override {
    activeComponentCount--;
    components[index] = std::move(components[activeComponentCount]);
    entityIdList[index] = entityIdList[activeComponentCount];

    return entityIdList[index];
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
};
