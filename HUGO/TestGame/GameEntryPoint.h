#pragma once

#ifdef _WIN32
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C" __attribute__((visibility("default")))
#endif

#include <memory>

EXPORT void FabricateComponent();