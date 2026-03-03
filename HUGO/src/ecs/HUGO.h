#pragma once
/*
    Este header está pensado para se incluido como desarrollador, tiene todo lo que se necesita
    para desarrollar componentes y sistemas
*/

#include <string>

#include "Component.h"

struct ComponentRegistry {
    static void add(const std::string& name) {
        get().push_back(name);
    }

    static const std::vector<std::string>& list() {
        return get();
    }

    private:
    static std::vector<std::string>& get() {
        static std::vector<std::string> v; // init seguro
        return v;
    }
};

#define HCOMPONENT(COMPONENT_NAME) \
    class COMPONENT_NAME: Component { \
    public: \
        static inline const std::string name = #COMPONENT_NAME; \
    \
    private: \
        struct COMPONENT_NAME##_AutoRegisterer { \
            COMPONENT_NAME##_AutoRegisterer() { \
                ComponentRegistry::add(#COMPONENT_NAME); \
            } \
        }; \
        static COMPONENT_NAME##_AutoRegisterer registerer;