#include "Scene.h"

#include "Ecs.h"
#include <utility>
#include <vector>

#include "../components/messages/CreateOgreObjectComponent.h"

Scene::Scene()
{}

void 
Scene::update(double dt) {
  for(System system : systems) 
    system.update(dt);
}

void 
Scene::fixedUpdate(double dt) {
  for(System system : systems) 
    system.fixedUpdate(dt);
}

EntityID 
Scene::createEntity() {
  return entityTable.createEntity();
}

void
Scene::removeEntity(EntityID entity) {
  // Deleting the components that the entity had
  std::vector<std::pair<ComponentID, DenseListIndex>> componentsToDelete = entityTable.getAllDenseListIndexes(entity);
  for(auto [componentID, direction] : componentsToDelete)
    componentTable.removeComponent(componentID, direction);

  // Deleting the entity
  entityTable.removeEntity(entity);
}