#pragma once
/*
    Este header está pensado para se incluido como desarrollador, tiene todo lo que se necesita
    para desarrollar componentes y sistemas
*/

#include <string>
#include <functional>
#include <unordered_map>
#include <any>

#include "IScene.h"
#include "Component.h"

struct ModuleComponentFactory {

    using ComponentGenerator = std::function<void(IScene&, EntityID, std::vector<std::any>)>; 

    static std::unordered_map<std::string, ComponentGenerator> generators;

    static void add(const std::string& name, ComponentGenerator generator) {
        generators[name] = generator;
    }

    static void fabricate(const std::string& name, IScene& scene, EntityID ent, std::vector<std::any> args) {
        if (generators.count(name)) {
            generators[name](scene, ent, args);
        }
        else {
            // POR TERMINAR, FALTA THROW O ERRORES O LO QUE SEA
        }
    }
};

#define HCOMPONENT(COMPONENT_NAME) \
    struct COMPONENT_NAME: Component { \
    public: \
        static inline const std::string name = #COMPONENT_NAME; \
    \
    private: \
        struct COMPONENT_NAME##_AutoRegisterer { \
            COMPONENT_NAME##_AutoRegisterer() { \
                ModuleComponentFactory::add(#COMPONENT_NAME,\
                    [](IScene& scene, EntityID ent, std::vector<std::any> args) { \
                        scene.addComponent<COMPONENT_NAME>(ent); \
                    } \
                ); \
            } \
        }; \
        static COMPONENT_NAME##_AutoRegisterer registerer;

/// Define los miembros que se pueden crear por los mapas
#define INIT_FIELDS(...) \
    public: \
        auto initialization_tuple() { return std::tie(__VA_ARGS__); } \
        auto initialization_tuple() const { return std::tie(__VA_ARGS__); } \
    \
        void init(const std::vector<std::any>& params) { \
            auto tup = initialization_tuple(); \
            constexpr size_t s = std::tuple_size<decltype(tup)>::value; \
            if (params.size() != s) \
                throw std::runtime_error("Numero de parámetros incorrecto"); \
            apply_init(tup, params, std::make_index_sequence<s>{}); \
        } \
    private: \
        template<typename Tuple, std::size_t... I> \
        void apply_init(Tuple& t, const std::vector<std::any>& params, std::index_sequence<I...>) { \
            ((std::get<I>(t) = std::any_cast<std::remove_reference_t<decltype(std::get<I>(t))>>(params[I])), ...); \
        } 


HCOMPONENT(MiComponent)
    std::string a;
    int b;

    INIT_FIELDS(a, b)
};