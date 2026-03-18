#include "Scene.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "EcsTypes.h"
#include <bitset>

#include "HugoError.h"
#include "HugoTypes.h"
#include "LuaBinder.h"

#include "EntityTable.h"
#include "ComponentDenseTable.h"

#include "ComponentFactory.h"
#include "ComponentRegistry.h"

Scene::Scene() {
	entityTable = std::make_unique<EntityTable>();
	componentTable = std::make_unique<ComponentDenseTable>();
}

//Load method is called when the scene is loaded so LUA methods are overwritten
void Scene::load() {
	LuaBinder& luaBinder = LuaBinder::Instance();
	luaBinder.registerMethod<Scene, void, int>(*this, &Scene::removeEntityLUA, "RemoveEntity");
	luaBinder.registerMethod<Scene, int>(*this, &Scene::createEntityLUA, "CreateEntity");

	for(std::shared_ptr<System> system : systems) {
		system->onLoad();
	}
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
  	return static_cast<EntityID>(entityTable->createEntity());
}

int 
Scene::createEntityLUA() {
  return createEntity();
}

void
Scene::addComponentLUA(int entityID, std::string& componentName) {
	//TO_CHAT problema con el uso de addComponent por las funciones templatizadas, hablar con Marco
}

void
Scene::removeComponentLUA(int entityID, std::string& componentName) {
	//TO_CHAT problema con el uso de removeComponent por las funciones templatizadas, hablar con Marco
}

void
Scene::removeEntity(EntityID entity) {
	// Deleting the components that the entity had
	std::vector<std::pair<ComponentID, DenseListIndex>> componentsToDelete = entityTable->getAllDenseListIndexes(entity);
	for(auto [componentID, direction] : componentsToDelete)
		componentTable->removeComponent(componentID, direction);

	// Deleting the entity
	entityTable->removeEntity(entity);
}

void
Scene::removeEntityLUA(int entity) {
  	removeEntity(static_cast<EntityID>(entity));
}

void 
Scene::printEntity(int id) {
	std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable->getComponentMask();

	//Imprime la bitmask de todas las entidad de forma provisional
	//TO_CHAT: Imprimir el nombre de la entidaa y sus componentes

	//console->Print("ENTITY WITH ID: " + std::to_string(id) + " has components " + masks[id].to_string() + "\n");
}

void 
Scene::printEntities() {
	std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable->getComponentMask();

	//Imprime la bitmask de todas las entidades de forma provisional
	//TO_CHAT: Imprimir los nombres de las entidades y sus componentes

	int index = 0;
	for(std::bitset mask : masks) {
		//console->Print("ENTITY WITH ID: " + std::to_string(index) + " has components " + mask.to_string() + "\n");
		index += 1;
	}
}

void 
Scene::log(std::shared_ptr<Error> error) {
    errorsToLog.push(error);
}

void 
Scene::log(HUGO::String log) {
    stringsToLog.push(log);
}

ErrorOrType<HUGO::String> 
Scene::getNextError() {
    if(errorsToLog.empty()) return ErrorOrType<HUGO::String>(StringError{"La cola está vacía"});

    HUGO::String ret = errorsToLog.front()->description();
    errorsToLog.pop();
    return ErrorOrType<HUGO::String>(ret);
}

ErrorOrType<HUGO::String> 
Scene::getNextLog() {
    if(stringsToLog.empty()) return ErrorOrType<HUGO::String>(StringError{"La cola está vacía"});

    HUGO::String ret = stringsToLog.front();
    stringsToLog.pop();
    return ErrorOrType<HUGO::String>(ret);
}
