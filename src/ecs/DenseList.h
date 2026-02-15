#pragma once

#include <vector>

#include "Ecs.h"
#include "Component.h"

/**
  * @brief Middleground interface for the generic dense list
*/
class IDenseList {
public:
  virtual DenseListIndex getCount() const = 0;
  virtual EntityID remove(DenseListIndex index) = 0;
  virtual void removeAllComponents() = 0;
};

template<typename T>
class DenseList : public IDenseList {
  std::vector<T> components;
  std::vector<EntityID> idList;
  DenseListIndex activeComponentCount;

public:
  DenseList() : components(), activeComponentCount(0u)
  {}

  DenseListIndex addComponent(const T& component, EntityID entity) {
    if(activeComponentCount == components.size()) 
      components.push_back(component);

    else
      components[activeComponentCount] = component;

    activeComponentCount++;

    return activeComponentCount - 1;
  }

  DenseListIndex getCount() const override {
    return activeComponentCount;
  }

  EntityID remove(DenseListIndex index) override {
    activeComponentCount--;
    components[index] = std::move(components[activeComponentCount]);
    idList[index] = idList[activeComponentCount];

    return idList[index];
  }

  T& getComponent(DenseListIndex index) {
    if(index >= activeComponentCount) throw "Out of range";

    return components[index];
  }
};
