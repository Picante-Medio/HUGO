#pragma once

#include <utility>
#include <vector>

#include "Ecs.h"
#include "ComponentDenseTable.h"
#include "EntityTable.h"
#include "Query.h"
#include "System.h"

class Scene {
  EntityTable entityIndexes;
  ComponentDenseTable componentTable;
  std::vector<System> systems;


  template<typename T>
  T& getComponent(EntityID entity) {
    ComponentID componentID = getComponentId<T>();
    return componentTable.getComponent<T>(entityIndexes.getDenseListIndex(entity, componentID));
  }

public:
  Scene();

  // Logic update
  void update(double dt);

  // Physics update
  void fixedUpdate(double dt);

  // Entity handling
  EntityID createEntity();
  void removeEntity(EntityID entity);

  // Component handling
  template<typename T, typename ...Ts>
  void addComponent(EntityID entity, Ts &&... args) {
    T component(entity, std::forward<Ts>(args)...);
    ComponentID id = getComponentId<T>();

    entityIndexes.registerComponent(entity, id, componentTable.addComponent(component, entity));
  }

  template<typename T>
  void removeComponent(EntityID entity) {
    ComponentID id = getComponentId<T>();
    DenseListIndex oldIndex = entityIndexes.getDenseListIndex(entity, id);
    entityIndexes.deRegisterComponent(entity, id);
    EntityID toUpdate = componentTable.removeComponent<T>();
    entityIndexes.updateDenseListIndex(toUpdate, id, oldIndex);
  }

  template<typename ...Ts>
  Query<Ts...> query() {
    Query<Ts...> returnedQuery;

    // Expandir los parámetros del pack para obtener las N denselists
    std::vector<ComponentID> ids = getAllComponentIds<Ts...>();

    int smallestListIndex = 0;
    DenseListIndex smallestSize = componentTable.getCount(ids[0]);
    for(int i = 1; i < ids.size(); ++i)
      if(componentTable.getCount(ids[i]) < smallestSize) smallestListIndex = i;
    ComponentID componentID = ids[smallestListIndex];

    std::vector<EntityID> potentialEntities = componentTable.affectedEntities(componentID);
    for(EntityID entity : potentialEntities) {
      bool affected = true;
      for(ComponentID id : ids) {
        if(!entityIndexes.hasComponent(entity, id)) {
          affected = false; 
          break;
        }

        if(affected) returnedQuery.push_back(getComponent<Ts>(entity)...);
      }
    }

    return returnedQuery;
  }
};
