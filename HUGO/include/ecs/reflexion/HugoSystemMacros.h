#pragma once
#include "IScene.h"
#include "SystemFactory.h"

#define HSYSTEM(SYSTEM_NAME) \
class SYSTEM_NAME : public HUGO::System { \
public:\
    SYSTEM_NAME(std::shared_ptr<HUGO::IScene> p_scene): HUGO::System(p_scene) {} \
    static inline const std::string name = #SYSTEM_NAME; \
private:\
    struct Reflection{ \
        Reflection(){\
            HUGO::SystemFactory::add(#SYSTEM_NAME, \
            [](std::shared_ptr<HUGO::IScene> p_scene) -> ErrorOrType<std::shared_ptr<HUGO::System>> { \
                return ErrorOrType<std::shared_ptr<HUGO::System>>(std::make_shared<SYSTEM_NAME>(SYSTEM_NAME(p_scene)));\
            });\
        }\
    };\

#define END_HSYSTEM() \
inline static Reflection reflection_instance_; \
};