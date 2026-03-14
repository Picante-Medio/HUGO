#pragma once
#include "Scene.h"
#include "SystemFactory.h"

#define HSYSTEM(SYSTEM_NAME) \
class SYSTEM_NAME : System { \
    SYSTEM_NAME(std::shared_ptr<Scene> scene): System(scene) {} \
    static inline const std::string name = #SYSTEM_NAME; \
    \
    struct Reflection{ \
        Reflection(){\
            SystemFactory::add(#SYSTEM_NAME, \
            [](std::shared_ptr<Scene> scene) -> ErrorOrType<std::shared_ptr<System>> { \
                return ErrorOrType<std::shared_ptr<System>>(std::make_shared<System>(SYSTEM_NAME(scene)));\
            });\
        }\
    };\

#define END_HSYSTEM() \
inline static Reflection reflection_instance_; \
};