#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "IScene.h"
#include "HugoError.h"
#include "HugoParsingVariant.h"

namespace HUGO {

struct ComponentFactory {
    
    using ComponentGenerator = std::function<ErrorOrVoid(IScene&, EntityID, std::unordered_map<std::string, HVariant>)>; 

    static std::unordered_map<std::string, ComponentGenerator>& generators() {
        static std::unordered_map<std::string, ComponentGenerator> generatorsInstance;
        return generatorsInstance;
    }

    static void add(const std::string& name, ComponentGenerator generator) {
        generators()[name] = generator;
    }

    static ErrorOrVoid makeComponent(IScene& scene, EntityID ent,
        const std::string& name, std::unordered_map<std::string, HVariant> args) {

        if (generators().count(name)) {
            return generators()[name](scene, ent, args);
        }

        StringError err;
        err.errorMessage = "El componente solicitado no existe o no ha sido registrado para fabricar";
        return ErrorOrVoid(err);
    }
};

}