#include "Scene.h"

#include <cstddef>
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
#include "GameLibraryLoader.h"

#include "StringUtils.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"

Scene::Scene(std::shared_ptr<HUGO::IInput> input) : input(input) {
	entityTable = std::make_unique<EntityTable>();
	componentTable = std::make_unique<ComponentDenseTable>();
	messenger = HUGO::Messenger();
}

//Load method is called when the scene is loaded so LUA methods are overwritten
void Scene::load() {
	LuaBinder& luaBinder = LuaBinder::Instance();
	luaBinder.registerMethod<Scene, void, int>(*this, &Scene::removeEntityLUA, "RemoveEntity");
	luaBinder.registerMethod<Scene, int>(*this, &Scene::createEntityLUA, "CreateEntity");

	for(std::shared_ptr<HUGO::System> system : systems) {
		system->onLoad();
	}
}

void 
Scene::update(double dt) {
	for(std::shared_ptr<HUGO::System> system : systems) 
		system->update(dt);
}

void 
Scene::fixedUpdate(double dt) {
	for(std::shared_ptr<HUGO::System> system : systems) 
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
Scene::addComponentLUA(int entityID, std::string& componentData) {
	//Input Example
	//AddComponent Rigidbody float mass 2.5 vector3 velocity 2 2 2
	
	std::vector<std::string_view> splittedData = StringUtils::split(componentData, ' ');
	std::string componentName = std::string{splittedData[1]};

	std::unordered_map<std::string, HVariant> parameters = std::unordered_map<std::string, HVariant>();

	std::string parameterNameBuffer = "";
	std::string_view parameterType = "";
	ErrorOrVoid error = ErrorOrVoid();

	for(size_t c = 2; c < splittedData.size(); c++) {
		std::string stringValue = std::string{splittedData[c]};
		HVariant parameterValue = "NULL";
		bool valueAssigned = false;

		if(parameterType == "") parameterType = stringValue;
		else if(parameterNameBuffer == "") parameterNameBuffer = stringValue;
		else if(parameterType == "float") parameterValue = static_cast<HUGO::Float>(std::stof(stringValue));
		else if(parameterType == "int") parameterValue = static_cast<HUGO::Int>(std::stoi(stringValue));
		else if(parameterType == "string") parameterValue = static_cast<HUGO::String>(stringValue);
		else if(parameterType == "vector2") {
			float x = std::stof(stringValue);
			float y = 0;

			if(c+1 >= splittedData.size()) error = ErrorOrVoid(StringError{"Faltan argumentos para Vector2"});
			else y = std::stof(std::string{splittedData[c]});

			parameterValue = HUGO::Vector2(x, y);
			c++;
		}
		else if(parameterType == "vector3") {
			float x = std::stof(stringValue);
			float y = 0;
			float z = 0;
			
			if(c+1 >= splittedData.size()) error = ErrorOrVoid(StringError{"Faltan argumentos para Vector2"});
			else y = std::stof(std::string{splittedData[c]});

			if(c+2 >= splittedData.size()) error = ErrorOrVoid(StringError{"Faltan argumentos para Vector2"});
			else y = std::stof(std::string{splittedData[c]});

			parameterValue = HUGO::Vector3(x, y, z);
			c += 2;
		}

		if(valueAssigned) {
			parameters[parameterNameBuffer] = parameterValue;
			parameterNameBuffer = "";
			parameterType = "";
		} 
	}

	if(error.isError()) {
		log(error.getError().errorMessage);
		return;
	}

	ErrorOrVoid compResult = HUGO::ComponentFactory::makeComponent(shared_from_this(), static_cast<EntityID>(entityID), componentName, parameters);
	if(compResult.isError()) {
		compResult = HUGO::GameLibraryLoader::instance().GenerateComponent(shared_from_this(), static_cast<EntityID>(entityID), componentName, parameters);
		if(compResult.isError()) {
			log(compResult.getError().errorMessage);
		}
	}
}

void
Scene::removeComponentLUA(int entityID, std::string& componentName) {
	ErrorOrVoid result = HUGO::ComponentFactory::removeComponent(shared_from_this(), entityID, componentName);

	if(result.isError()) {
		log(result.getError().errorMessage);
	}
}

void
Scene::removeEntity(EntityID entity) {
	//if the entity has children, remove them
	if(hasComponent<ChildrenComponent>(entity)){
		std::unordered_set<EntityID> children = getComponent<ChildrenComponent>(entity).getChildren();
		for(EntityID child : children){
			removeEntity(child);
		}
	}

	//notify parent
	if(hasComponent<ParentComponent>(entity)){
		std::optional<EntityID> parent = getComponent<ParentComponent>(entity).getParent();
		if(parent && hasComponent<ChildrenComponent>(parent.value())){
			getComponent<ChildrenComponent>(parent.value()).removeChild(entity);
		}
	}
	
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
	log(static_cast<HUGO::String>("ENTITY WITH ID: " + std::to_string(id) + " has components " + masks[id].to_string() + "\n"));
}

void 
Scene::printEntities() {
	std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable->getComponentMask();

	//Imprime la bitmask de todas las entidades de forma provisional
	//TO_CHAT: Imprimir los nombres de las entidades y sus componentes en un formato legible

	int index = 0;
	for(std::bitset mask : masks) {
		log(static_cast<HUGO::String>("ENTITY WITH ID: " + std::to_string(index) + " has components " + mask.to_string() + "\n"));
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

void
Scene::dispatchMessages() {
	messenger.sendAll();
}

std::shared_ptr<HUGO::IInput>
Scene::getInput() {
	return input;
}
