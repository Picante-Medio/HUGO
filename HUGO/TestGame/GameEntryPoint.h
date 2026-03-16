#pragma once

#include "IScene.h"
#include <string>
#include <HugoParsingVariant.h>
#include <unordered_map>

#ifdef _WIN32
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C" __attribute__((visibility("default")))
#endif

#include <memory>

EXPORT bool GenerateComponent(std::shared_ptr<IScene> scene, const std::string& component, 
        const std::unordered_map<std::string, HVariant>& componentMembers);