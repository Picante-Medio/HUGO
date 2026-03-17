#pragma once

#include "ComponentRegistry.h"

class ComponentRegistryInitializer: public ComponentRegistry{
public:
    ComponentRegistryInitializer() = default;

    static std::shared_ptr<ComponentRegistry> getInstance() {

		if (instance == nullptr) {
			instance = std::make_shared<ComponentRegistryInitializer>();
		}

		return instance;
	}
};