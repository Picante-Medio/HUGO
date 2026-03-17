#pragma once

#include "ComponentRegistry.h"

class ComponentRegistryInitializer: public ComponentRegistry{
public:
    ComponentRegistryInitializer() = default;

    static std::shared_ptr<ComponentRegistry> getInstance() {

		if (ComponentRegistry::instance == nullptr) {
			ComponentRegistry::instance = std::make_shared<ComponentRegistryInitializer>();
		}

		return instance;
	}
};