#pragma once

#include <unordered_map>
#include "EcsTypes.h"

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
			throw std::invalid_argument("Component name is empty");
		}
		else {
			if (registry.count(componentName)) {
				return registry[componentName];
			}
			else {
				if(currentId == MAX_COMPONENTS) {
					throw std::overflow_error("MAX_COMPONENTS reached");
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
		if (instance == nullptr) {
			instance = registry;
		}
	}
};