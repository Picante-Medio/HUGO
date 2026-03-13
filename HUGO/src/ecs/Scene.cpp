#include "Scene.h"

#include <memory>
#include <utility>
#include <vector>
#include "Ecs.h"

#include "LuaBinder.h"

Scene::Scene() {
  LuaBinder& luaBinder = LuaBinder::Instance();
  luaBinder.registerMethod<Scene, void, int>(*this, &Scene::removeEntityLUA, "RemoveEntity");
  luaBinder.registerMethod<Scene, int>(*this, &Scene::createEntityLUA, "CreateEntity");
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
  return static_cast<EntityID>(entityTable.createEntity());
}

int 
Scene::createEntityLUA() {
  return createEntity();
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

void
Scene::removeEntityLUA(int entity) {
  removeEntity(static_cast<EntityID>(entity));
}


void Scene::printEntities() {
}