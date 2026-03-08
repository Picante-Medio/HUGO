#pragma once

#include "ComponentRegistry.h"

class ComponentRegistryInitializer: public ComponentRegistry{
public:
    ComponentRegistryInitializer() = default;

    static ComponentRegistry& getInstance() {
		return *instance;
	}
};