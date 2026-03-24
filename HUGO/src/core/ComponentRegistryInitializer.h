#pragma once

#include "ComponentRegistry.h"

class ComponentRegistryInitializer: public HUGO::ComponentRegistry{
public:
    ComponentRegistryInitializer() = default;

    static std::shared_ptr<HUGO::ComponentRegistry> getInstance() {

		if (HUGO::ComponentRegistry::instance == nullptr) {
			HUGO::ComponentRegistry::instance = std::make_shared<ComponentRegistryInitializer>();
		}

		return instance;
	}
};