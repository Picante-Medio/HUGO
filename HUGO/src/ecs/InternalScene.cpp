#include "InternalScene.h"

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
#include "SceneManager.h"
#include "ComponentFactory.h"
#include "GameLibraryLoader.h"
#include "HStringUtils.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"

InternalScene::InternalScene(std::shared_ptr<HUGO::IInput> input, HUGO::SceneManager& sceneManager) 
    : input(input), sceneManager(sceneManager)
{
	entityTable = std::make_unique<EntityTable>();
	componentTable = std::make_unique<ComponentDenseTable>();
}

//Load method is called when the scene is loaded so LUA methods are overwritten
void InternalScene::load() {
    LuaBinder& luaBinder = LuaBinder::getInstance();
	luaBinder.registerMethod<InternalScene, void, int>(*this, &InternalScene::removeEntityLUA, "RemoveEntity");
	luaBinder.registerMethod<InternalScene, int>(*this, &InternalScene::createEntityLUA, "CreateEntity");

	for(std::shared_ptr<HUGO::System> system : systems) {
		system->onLoad();
	}
}

void InternalScene::init(){
	for(std::shared_ptr<HUGO::System> system : systems) {
		system->init();
	}
}

void 
InternalScene::update(uint32_t dt) {
	for(std::shared_ptr<HUGO::System> system : systems) 
		system->update(dt);
}

void 
InternalScene::fixedUpdate(uint32_t dt) {
	for(std::shared_ptr<HUGO::System> system : systems) 
		system->fixedUpdate(dt);
}

EntityID 
InternalScene::createEntity() {
  	return static_cast<EntityID>(entityTable->createEntity());
}

int 
InternalScene::createEntityLUA() {
  return createEntity();
}

void
InternalScene::addComponentLUA(int entityID, std::string& componentData) {

	//Input Example
	//AddComponent Rigidbody float mass 2.5 vector3 velocity 2 2 2
	
	std::vector<std::string_view> splittedData = HStringUtils::split(componentData, ' ');
	std::string componentName = std::string{splittedData[1]};

	std::unordered_map<std::string, HVariant> parameters = std::unordered_map<std::string, HVariant>();

	std::string parameterNameBuffer = "";
	std::string_view parameterType = "";
	HUGO::ErrorOrVoid error = HUGO::ErrorOrVoid();

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

			if(c+1 >= splittedData.size()) error = HUGO::ErrorOrVoid(HUGO::StringError{"Faltan argumentos para Vector2"});
			else y = std::stof(std::string{splittedData[c]});

			parameterValue = HUGO::Vector2(x, y);
			c++;
		}
		else if(parameterType == "vector3") {
			float x = std::stof(stringValue);
			float y = 0;
			float z = 0;
			
			if(c+1 >= splittedData.size()) error = HUGO::ErrorOrVoid(HUGO::StringError{"Faltan argumentos para Vector2"});
			else y = std::stof(std::string{splittedData[c]});

			if(c+2 >= splittedData.size()) error = HUGO::ErrorOrVoid(HUGO::StringError{"Faltan argumentos para Vector2"});
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

	HUGO::ErrorOrVoid compResult = HUGO::ComponentFactory::makeComponent(shared_from_this(), static_cast<EntityID>(entityID), componentName, parameters);
	if(compResult.isError()) 
		log(compResult.getError().errorMessage);
}

void
InternalScene::removeComponentLUA(int entityID, std::string& componentName) {
	HUGO::ErrorOrVoid result = HUGO::ComponentFactory::removeComponent(shared_from_this(), entityID, componentName);

	if(result.isError()) {
		log(result.getError().errorMessage);
	}
}

void
InternalScene::removeEntity(EntityID entity) {
	// If the entity has children, remove them
	if(hasComponent<ChildrenComponent>(entity)){
		std::unordered_set<EntityID> children = getComponent<ChildrenComponent>(entity).getChildren();
		for(EntityID child : children){
			removeEntity(child);
		}
	}

	// If the entity had a parent, notify them
	if(hasComponent<ParentComponent>(entity)){
		std::optional<EntityID> parent = getComponent<ParentComponent>(entity).getParent();
		if(parent && hasComponent<ChildrenComponent>(parent.value())){
			getComponent<ChildrenComponent>(parent.value()).removeChild(entity);
		}
	}
    
	// Delete the components that the entity had
	std::vector<std::pair<ComponentID, DenseListIndex>> componentsToDelete = entityTable->getAllDenseListIndexes(entity);
	for(auto [componentID, direction] : componentsToDelete)
        removeComponentAux(entity, componentID, direction);
		
	// Delete the entity
	entityTable->removeEntity(entity);
}

void
InternalScene::removeEntityLUA(int entity) {
  	removeEntity(static_cast<EntityID>(entity));
}

void 
InternalScene::printEntity(int id) {
	std::vector<std::bitset<MAX_COMPONENTS>> masks = entityTable->getComponentMask();

	//Imprime la bitmask de todas las entidad de forma provisional
	log(static_cast<HUGO::String>("ENTITY WITH ID: " + std::to_string(id) + " has components " + masks[id].to_string() + "\n"));
}

void 
InternalScene::printEntities() {
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
InternalScene::log(std::shared_ptr<HUGO::Error> error) {
    errorsToLog.push(error);
}

void 
InternalScene::log(HUGO::String log) {
    stringsToLog.push(log);
}

HUGO::ErrorOrType<HUGO::String> 
InternalScene::getNextError() {
    if(errorsToLog.empty()) return HUGO::ErrorOrType<HUGO::String>(HUGO::StringError{"La cola está vacía"});

    HUGO::String ret = errorsToLog.front()->description();
    errorsToLog.pop();
    return HUGO::ErrorOrType<HUGO::String>(ret);
}

HUGO::ErrorOrType<HUGO::String> 
InternalScene::getNextLog() {
    if(stringsToLog.empty()) return HUGO::ErrorOrType<HUGO::String>(HUGO::StringError{"La cola está vacía"});

    HUGO::String ret = stringsToLog.front();
    stringsToLog.pop();
    return HUGO::ErrorOrType<HUGO::String>(ret);
}

void
InternalScene::dispatchMessages() {
	messenger.sendAll();
}

std::shared_ptr<HUGO::IInput>
InternalScene::getInput() {
	return input;
}


void 
InternalScene::removeComponentAux(EntityID entity, ComponentID componentID, DenseListIndex index) {
    // Calls removal event before removing
    componentTable->dispatchRemoveEvent(componentID, index);

    // Deleting the component
    entityTable->deRegisterComponent(entity, componentID);
    auto [needsUpdate, toUpdate] = componentTable->removeComponent(componentID, index);

    // Updating the index of the moved component upon deletion
    if(needsUpdate) entityTable->updateDenseListIndex(toUpdate, componentID, index);
}

void 
InternalScene::changeScene(const HUGO::String& sceneName) {
    sceneManager.setCurrentScene(sceneName);
}

void 
InternalScene::changeAndDestroyPreviousScene(const HUGO::String& sceneName) {
    sceneManager.setCurrentSceneAndDestroyPrevious(sceneName);
}

HUGO::ErrorOrType<EntityID> InternalScene::instantiatePrefab(const HUGO::String& prefabName){
	HUGO::ErrorOrType<EntityID> prefabID = sceneManager.instantiatePrefab(prefabName, shared_from_this());

	if(prefabID.isError()) log(std::make_shared<HUGO::StringError>("Error instantiating prefab " + prefabName + ": "+ prefabID.getError().description()));

	return prefabID;
}
