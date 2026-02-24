#pragma once

#include <memory>
#include <vector>
#include <array>

#include "Ecs.h"
#include "DenseList.h"


/**
  * @brief a list of DenseList objects, each DenseList of a different type of component
  */
class ComponentDenseTable {
  std::array<std::shared_ptr<IDenseList>, MAX_COMPONENTS> componentLists; // Components packed and alligned in memory

  /**
  * @brief auxiliar function that creates a DenseList of the specified type if it does not exist yet
  * @tparam the type of component that the casted list stores
  */
  template<typename T>
  void ensureListExists() {
    ComponentID componentID = getComponentId<T>();
    if(componentLists[componentID] != nullptr) return;
    componentLists[componentID] = std::make_unique<DenseList<T>>();
  }

  /**
  * @brief auxiliar function that casts an IDenseList to a DenseList of a certain type
  * @tparam the type of component that the casted list stores
  * @return a pointer to the specified type of dense list
  */
  template<typename T>
  std::shared_ptr<DenseList<T>> getComponentList() {
    ensureListExists<T>();
    ComponentID cmpID = getComponentId<T>();
    return std::static_pointer_cast<DenseList<T>>(componentLists[cmpID]);
  }

public:
  ComponentDenseTable(); 

  /**
  * @brief calls removeComponent on the DenseList that stores components of id componentID
  * @param componentID id of the component to be removed
  * @param index index in the denselist of the component to be removed
  * @return the id of the entity which component has moved to preserve order. The new index of said component is the value of the index parameter
  */
  EntityID removeComponent(ComponentID componentID, DenseListIndex index);
  DenseListIndex getCount(ComponentID componentID) const;

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
  EntityID removeComponent(DenseListIndex index) {
    return getComponentList<T>()->remove(index);
  };


  /**
  * @brief calls getCount on the DenseList that stores components of type T
  * @tparam the type of the removed component
  * @return the number of valid components stored in the DenseList
  */
  template<typename T>
  DenseListIndex getCount() const {
    return getCount(getComponentId<T>());
  }

  std::vector<EntityID> affectedEntities(ComponentID componentID); 
};
