#pragma once

#include <memory>
#include <vector>
#include <array>

#include "Ecs.h"
#include "DenseList.h"

class ComponentDenseTable {
  std::array<std::unique_ptr<IDenseList>, MAX_COMPONENTS> componentLists; // Components packed and alligned in memory
  std::array<ComponentIndex, MAX_COMPONENTS> activeSize; // Size of the vector of alive components

  template<typename T>
  std::unique_ptr<DenseList<T>> getComponentList() {
    ComponentID cmpID = getComponentId<T>();
    std::unique_ptr<DenseList<T>> list = static_cast<std::unique_ptr<DenseList<T>>>(componentLists[cmpID]);
  }

public:
  ComponentDenseTable();

  EntityID removeComponent(ComponentID id, ComponentIndex index);
  ComponentIndex getCount(ComponentID id) const;

  template<typename T>
  T& getComponent(ComponentIndex index) {
    return getComponentList<T>()->getComponent(index);
  }

  template<typename T>
  ComponentIndex addComponent(const T& component, EntityID entity) {
    return getComponentList<T>()->addComponent(component, entity);
  }

  template<typename T>
  EntityID removeComponent(ComponentIndex direction) {
    removeComponent(getComponentId<T>(), direction);
  };

  template<typename T>
  T& getCount() const {
    return getCount(getComponentId<T>());
  }
};
