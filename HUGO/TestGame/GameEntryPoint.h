#pragma once

#include "HUGO.h"
#include <string>
#include <unordered_map>

#ifdef _WIN32
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C" __attribute__((visibility("default")))
#endif

#include <memory>

EXPORT void InitializeGame(std::shared_ptr<ComponentRegistry> registry);

EXPORT bool GenerateComponent(std::shared_ptr<IScene>, EntityID ent, const std::string& component, 
    const std::unordered_map<std::string, HVariant>& componentMembers);

EXPORT bool GenerateAndGetSystem(std::shared_ptr<IScene> scene, const std::string& systemName, std::shared_ptr<System>& returnSystem);