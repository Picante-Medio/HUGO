#pragma once

#include <memory>

#include "ComponentFactory.h"
#include "Component.h"
#include "FieldData.h"

#define HCOMPONENT(COMPONENT_NAME) \
    struct COMPONENT_NAME: Component { \
        COMPONENT_NAME(EntityID ent): Component(ent) {} \
        static inline const std::string name = #COMPONENT_NAME; \
        using ThisComponent = COMPONENT_NAME; \
        struct Reflection { \
            static std::unordered_map<std::string, std::unique_ptr<IFieldData<COMPONENT_NAME>>>& members() { \
                static std::unordered_map<std::string, std::unique_ptr<IFieldData<COMPONENT_NAME>>> inst; \
                return inst; \
            } \
            Reflection() { \
                HUGO::ComponentFactory::add(#COMPONENT_NAME, \
                    [](IScene& scene, EntityID ent, std::unordered_map<std::string, HVariant> args) -> ErrorOrVoid { \
                        COMPONENT_NAME& c = scene.addAndGetComponent<COMPONENT_NAME>(ent); \
                        for(auto& [key,val] : args) { \
                            auto it = members().find(key); \
                            if(it != members().end()) it->second->Set(c,val); \
                            else { \
                                StringError err; \
                                err.errorMessage = "No existe un miembro " + key; \
                                return ErrorOrVoid(err); \
                            } \
                        } \
                        return ErrorOrVoid(); \
                }); \
            } \
        }; \

#define SERIALIZED_MEMBER(TYPE, NAME) \
    TYPE NAME; \
    inline static struct Register##NAME_ { \
        Register##NAME_() { \
            ThisComponent::Reflection::members()[#NAME] = \
                std::make_unique<FieldData<ThisComponent, TYPE>>(&ThisComponent::NAME); \
        } \
    } auto_register_##NAME_;


#define DEFAULT_SERIALIZED_MEMBER(TYPE, NAME, INIT_VALUE) \
    TYPE NAME = INIT_VALUE; \
    inline static struct Register##NAME_ { \
        Register##NAME_() { \
            ThisComponent::Reflection::members()[#NAME] = \
                std::make_unique<FieldData<ThisComponent, TYPE>>(&ThisComponent::NAME); \
        } \
    } auto_register_##NAME_;

#define END_HCOMPONENT() \
    inline static Reflection reflection_instance_; \
};