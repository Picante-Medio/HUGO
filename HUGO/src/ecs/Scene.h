#pragma once

#include <utility>
#include <vector>

#include "ComponentDenseTable.h"
#include "Ecs.h"
#include "EntityTable.h"
#include "Query.h"
#include "System.h"

class Scene {
  EntityTable entityTable;
  ComponentDenseTable componentTable;
  std::vector<System> systems;

  template <typename T> T &getComponent(EntityID entity) {
    ComponentID componentID = getComponentId<T>();
    return componentTable.getComponent<T>(
        entityTable.getDenseListIndex(entity, componentID));
  }

public:
  Scene();

  inline void addSystem(System system) { systems.push_back(system); }

  // Logic update
  void update(double dt);

  // Physics update
  void fixedUpdate(double dt);

  // Entity handling
  EntityID createEntity();
  void removeEntity(EntityID entity);

  // Component handling
  template <typename T, typename... Ts>
  void addComponent(EntityID entity, Ts &&...args) {
    // Creating the component
    T component(entity, std::forward<Ts>(args)...);
    ComponentID id = getComponentId<T>();

    // Adding the component
    entityTable.registerComponent(
        entity, id, componentTable.addComponent(component, entity));
  }

  template <typename T> void removeComponent(EntityID entity) {
    // Obtaining the index of the component to delete
    ComponentID id = getComponentId<T>();
    DenseListIndex oldIndex = entityTable.getDenseListIndex(entity, id);

    // Deleting the component
    entityTable.deRegisterComponent(entity, id);
    EntityID toUpdate = componentTable.removeComponent<T>();

    // Updating the index of the moved component upon deletion
    entityTable.updateDenseListIndex(toUpdate, id, oldIndex);
  }

  template <typename... Ts> Query<Ts...> query() {
    Query<Ts...> returnedQuery;

    // Expand the parameter pack to get the affected components by ID
    std::vector<ComponentID> ids = getAllComponentIds<Ts...>();

    // Create the bitmask to avoid hasComponent operations
    std::bitset<MAX_COMPONENTS> queryMask;
    for (ComponentID id : ids)
      queryMask.set(id);

    // Get the smallest list of possible entities
    int smallestListIndex = 0;
    DenseListIndex smallestSize = componentTable.getCount(ids[0]);
    for (int i = 1; i < ids.size(); ++i)
      if (componentTable.getCount(ids[i]) < smallestSize)
        smallestListIndex = i;

    ComponentID componentID = ids[smallestListIndex];
    std::vector<EntityID> potentialEntities = componentTable.affectedEntities(componentID);

    // Add components of the right entities to the query
    for (EntityID entity : potentialEntities)
        if (entityTable.isInQuery(entity, queryMask))
          returnedQuery.push_back(getComponent<Ts>(entity)...);

    return returnedQuery;
  }

  void addRenderEntity(EntityID entity, const std::string& meshName, const std::string& materialName="");
};
