#include "Scene.h"
#include "Ecs.h"
#include <utility>
#include <vector>

Scene::Scene()
{}

void 
Scene::update(double dt) {}

void 
Scene::fixedUpdate(double dt) {}

EntityID 
Scene::createEntity() {
  return entityDirections.createEntity();
}

void
Scene::removeEntity(EntityID entity) {
  std::vector<std::pair<ComponentID, unsigned int>> componentsToDelete = entityDirections.getAllComponentIndexes(entity);

  for(auto [componentID, direction] : componentsToDelete)
    components.removeComponent(componentID, direction);
}
