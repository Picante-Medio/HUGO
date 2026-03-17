#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include "IScene.h"
#include "HugoError.h"
#include "HugoParsingVariant.h"

namespace HUGO {

class ComponentFactory {
public:
    using ComponentGenerator = std::function<ErrorOrVoid(std::shared_ptr<IScene>, EntityID, 
        const std::unordered_map<std::string, HVariant>&)>;

private:
    static std::unordered_map<std::string, ComponentGenerator>& generators() {
        static std::unordered_map<std::string, ComponentGenerator> generatorsInstance;
        return generatorsInstance;
    }

public:
    static void add(const std::string& name, ComponentGenerator generator) {
        generators()[name] = generator;
    }

    static ErrorOrVoid makeComponent(std::shared_ptr<IScene> scene, EntityID ent,
        const std::string& name, const std::unordered_map<std::string, HVariant>& args) {

        if (generators().count(name)) {
            generators()[name](scene, ent, args);
            return ErrorOrVoid();
        }

        StringError err;
        err.errorMessage = "El componente solicitado no existe o no ha sido registrado para fabricar";
        return ErrorOrVoid(err);
    }
};

}