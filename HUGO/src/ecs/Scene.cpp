#include "Scene.h"

#include <memory>
#include <utility>
#include <vector>
#include "Ecs.h"

Scene::Scene() {

}

void 
Scene::initSimulation() {
  simulation = std::make_unique<HUGO::Simulation>(shared_from_this());
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

void
Scene::stepPhysics(double dt) {
  simulation->stepSimulation(dt);
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
