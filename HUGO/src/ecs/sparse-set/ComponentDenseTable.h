#pragma once

#include "IComponentDenseTable.h"

/**
  * @brief a list of DenseList objects, each DenseList of a different type of component
  */
class ComponentDenseTable: public IComponentDenseTable {
public:
  ComponentDenseTable(); 

  /**
  * @brief calls removeComponent on the DenseList that stores components of id componentID
  * @param componentID id of the component to be removed
  * @param index index in the denselist of the component to be removed
  * @return the id of the entity which component has moved to preserve order. The new index of said component is the value of the index parameter
  */
  std::pair<bool, EntityID> removeComponent(ComponentID componentID, DenseListIndex index) override;
  
  DenseListIndex getCount(ComponentID componentID) const override;

  std::vector<EntityID> affectedEntities(ComponentID componentID) override;
};
