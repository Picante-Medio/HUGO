#include "Scene.h"

#include <memory>
#include <utility>
#include <vector>
#include "Ecs.h"
#include <bitset>

#include "LuaBinder.h"

Scene::Scene() {
  //DEPRECATED: this will be in load method
  LuaBinder& luaBinder = LuaBinder::Instance();
  luaBinder.registerMethod<Scene, void, int>(*this, &Scene::removeEntityLUA, "RemoveEntity");
  luaBinder.registerMethod<Scene, int>(*this, &Scene::createEntityLUA, "CreateEntity");

  console = std::make_shared<LuaConsole>();
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

void 
Scene::printEntity(int id) {
  std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable.getComponentMask();

  //Imprime la bitmask de todas las entidad de forma provisional
  //TODO: Imprimir el nombre de la entidaa y sus componentes

  console->Print("ENTITY WITH ID: " + std::to_string(id) + " has components " + masks[id].to_string() + "\n");
}

void 
Scene::printEntities() {
  std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable.getComponentMask();

  //Imprime la bitmask de todas las entidades de forma provisional
  //TODO: Imprimir los nombres de las entidades y sus componentes

  int index = 0;
  for(std::bitset mask : masks) {
    console->Print("ENTITY WITH ID: " + std::to_string(index) + " has components " + mask.to_string() + "\n");
    index += 1;
  }
}