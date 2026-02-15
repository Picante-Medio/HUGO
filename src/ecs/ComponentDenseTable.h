#pragma once

#include <memory>
#include <vector>
#include <array>

#include "Ecs.h"
#include "DenseList.h"

class ComponentDenseTable {
  std::array<std::unique_ptr<IDenseList>, MAX_COMPONENTS> componentLists; // Components packed and alligned in memory

  template<typename T>
  std::unique_ptr<DenseList<T>> getComponentList() {
    ComponentID cmpID = getComponentId<T>();
    std::unique_ptr<DenseList<T>> list = static_cast<std::unique_ptr<DenseList<T>>>(componentLists[cmpID]);
  }

  template<typename T>
  void ensureListExists() {
    ComponentID componentID = getComponentId<T>();
    if(componentLists[componentID] != nullptr) return;

    componentLists[componentID] = std::make_unique<DenseList<T>>();
  }

public:
  ComponentDenseTable(); 

  EntityID removeComponent(ComponentID id, DenseListIndex index);
  DenseListIndex getCount(ComponentID id) const;

  template<typename T>
  T& getComponent(DenseListIndex index) {
    ensureListExists<T>();
    return getComponentList<T>()->getComponent(index);
  }

  template<typename T>
  DenseListIndex addComponent(const T& component, EntityID entity) {
    ensureListExists<T>();
    return getComponentList<T>()->addComponent(component, entity);
  }

  template<typename T>
  EntityID removeComponent(DenseListIndex direction) {
    ensureListExists<T>();
    removeComponent(getComponentId<T>(), direction);
  };

  template<typename T>
  T& getCount() const {
    ensureListExists<T>();
    return getCount(getComponentId<T>());
  }
};
