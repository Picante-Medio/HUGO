#include "SystemFactory.h"

namespace HUGO {

std::unordered_map<std::string, SystemFactory::SystemGeneratorFn>& 
SystemFactory::generators() {
    static std::unordered_map<std::string, SystemGeneratorFn> generatorsInstance;
    return generatorsInstance;
}

void 
SystemFactory::add(const std::string& name, SystemGeneratorFn generator) {
    generators()[name] = generator;
}

ErrorOrType<std::shared_ptr<HUGO::System>> 
SystemFactory::makeSystem(HUGO::IScene& scene, const std::string& name) {

    if (generators().count(name)) {
        return generators()[name](scene);
    }

    StringError err;
    err.errorMessage = "Requested system doesn't exist or hasn't been registered for creation";
    return ErrorOrType<std::shared_ptr<HUGO::System>>(err);
}

}