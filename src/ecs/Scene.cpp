#include "Scene.h"

#include "Ecs.h"
#include <utility>
#include <vector>

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
  return entityIndexes.createEntity();
}

void
Scene::removeEntity(EntityID entity) {
  std::vector<std::pair<ComponentID, ComponentIndex>> componentsToDelete = entityIndexes.getAllComponentIndexes(entity);

  for(auto [componentID, direction] : componentsToDelete)
    componentTable.removeComponent(componentID, direction);

  entityIndexes.removeEntity(entity);
}
