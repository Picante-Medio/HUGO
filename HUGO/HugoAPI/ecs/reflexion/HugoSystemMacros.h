#pragma once
#include "IScene.h"
#include "SystemFactory.h"

#define HSYSTEM(SYSTEM_NAME) \
class SYSTEM_NAME : public HUGO::System { \
public:\
    SYSTEM_NAME(HUGO::IScene& sceneRef): HUGO::System(sceneRef) {} \
    static inline const std::string name = #SYSTEM_NAME; \
private:\
    struct Reflection{ \
        Reflection(){\
            HUGO::SystemFactory::add(#SYSTEM_NAME, \
            [](HUGO::IScene& sceneRef) -> HUGO::ErrorOrType<std::shared_ptr<HUGO::System>> { \
                return HUGO::ErrorOrType<std::shared_ptr<HUGO::System>>(std::make_shared<SYSTEM_NAME>(sceneRef)); \
            });\
        }\
    };\

#define END_HSYSTEM() \
inline static Reflection reflection_instance_; \
};