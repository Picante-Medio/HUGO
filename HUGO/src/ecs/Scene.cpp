#include "Scene.h"

#include <memory>
#include <utility>
#include <vector>
#include "Ecs.h"

#include "LuaBinder.h"

Scene::Scene() {
  LuaBinder& luaBinder = LuaBinder::Instance();
  luaBinder.registerMethod<Scene, void, EntityID>(*this, &Scene::removeEntity, "RemoveEntity");
  luaBinder.registerMethod<Scene, EntityID>(*this, &Scene::createEntity, "CreateEntity");
}

void 
Scene::update(double dt) {
  for(std::shared_ptr<System> system : systems) 
    system->update(dt);
}

void 
Scene::fixedUpdate(double dt) {
  for(std::shared_ptr<System> system : systems) 
    system->fixedUpdate(dt);
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
