#include <vector>

#include "Ecs.h"
#include "Component.h"

class IDenseList {
public:
  virtual ComponentIndex getCount() const = 0;
  virtual EntityID remove(ComponentIndex index) = 0;
  virtual void removeAllComponents() = 0;
};

template<typename T>
class DenseList : public IDenseList {
  std::vector<T> components;
  std::vector<EntityID> idList;
  ComponentIndex activeComponentCount;

public:
  DenseList() : components(), activeComponentCount(0u)
  {}

  ComponentIndex addComponent(const T& component, EntityID entity) {
    if(activeComponentCount == components.size()) 
      components.push_back(component);

    else
      components[activeComponentCount] = component;

    activeComponentCount++;

    return activeComponentCount - 1;
  }

  ComponentIndex getCount() const override {
    return activeComponentCount;
  }

  EntityID remove(ComponentIndex index) override {
    activeComponentCount--;
    components[index] = std::move(components[activeComponentCount]);
    idList[index] = idList[activeComponentCount];

    return idList[index];
  }

  T& getComponent(ComponentIndex index) {
    if(index >= activeComponentCount) throw "Out of range";

    return components[index];
  }
};
