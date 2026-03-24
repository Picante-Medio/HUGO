#pragma once
#include <unordered_map>
#include <functional>
#include "HugoError.h"
#include "IScene.h"
#include "System.h"

namespace HUGO {

class SystemFactory{

public:
    //Returns shared_ptr of the system because the Scene that holds it must reference it to update it.
    using SystemGenerator = std::function<ErrorOrType<std::shared_ptr<HUGO::System>>(std::shared_ptr<HUGO::IScene>)>; 

private:
    static std::unordered_map<std::string, SystemGenerator>& generators() {
        static std::unordered_map<std::string, SystemGenerator> generatorsInstance;
        return generatorsInstance;
    }

public:

    static void add(const std::string& name, SystemGenerator generator) {
        generators()[name] = generator;
    }

    static ErrorOrType<std::shared_ptr<HUGO::System>> makeSystem(std::shared_ptr<HUGO::IScene> scene,
        const std::string& name) {

        if (generators().count(name)) {
            return generators()[name](scene);
        }

        StringError err;
        err.errorMessage = "Requested system doesn't exist or hasn't been registered for creation";
        return ErrorOrType<std::shared_ptr<HUGO::System>>(err);
    }

};

}