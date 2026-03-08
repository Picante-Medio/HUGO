#pragma once

#include "HugoTypes.h"
#include "HugoError.h"
#include "Ecs.h"

class ComponentRegistry {
protected:

	ComponentRegistry() = default;

	ComponentID currentId = 0;
	std::unordered_map<std::string, ComponentID> registry = 
		std::unordered_map<std::string, ComponentID>();

	inline static std::shared_ptr<ComponentRegistry> instance = nullptr;

public:

	virtual ~ComponentRegistry() = default;

	ComponentID getRegistryComponentID(std::string componentName) {
		if (componentName.empty()) {
			StringError err;
			err.errorMessage = "El componente no tiene nombre";
			return MAX_COMPONENTS;
		}
		else {
			if (registry.count(componentName)) {
				return registry[componentName];
			}
			else {
				if(currentId == MAX_COMPONENTS) {
					return MAX_COMPONENTS;
				}
				registry[componentName] = currentId++;
				return registry[componentName];
			}
		}
	}

	template<typename T>
	static ComponentID getComponentId() {
		static ComponentID index = instance->getRegistryComponentID(T::name);
		return index;
	}

	template<typename... Ts>
	static std::vector<ComponentID> getAllComponentIds() {
		return { getComponentId<Ts>()... };
	}

	static void setInstance(std::shared_ptr<ComponentRegistry> registry) {
		instance = registry;
	}
};