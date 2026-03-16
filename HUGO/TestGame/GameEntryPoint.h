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

EXPORT bool GenerateComponent(IScene& scene, EntityID ent, const std::string& component, 
        const std::unordered_map<std::string, HVariant>& componentMembers);