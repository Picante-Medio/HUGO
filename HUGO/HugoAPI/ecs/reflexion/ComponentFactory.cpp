#include "ComponentFactory.h"

namespace HUGO {

    std::unordered_map<std::string, ComponentFactory::ComponentConstructorFn>& 
    ComponentFactory::constructors() {
        static std::unordered_map<std::string, ComponentFactory::ComponentConstructorFn> generatorsInstance;
        return generatorsInstance;
    }

    std::unordered_map<std::string, ComponentFactory::ComponentDestructorFn>& 
    ComponentFactory::destructors() {
        static std::unordered_map<std::string, ComponentFactory::ComponentDestructorFn> destructorsInstance;
        return destructorsInstance;
    }

    void
    ComponentFactory::addConstructor(const std::string& name, ComponentConstructorFn generator) {
        constructors()[name] = generator;
    }

    void 
    ComponentFactory::addDestructor(const std::string& name, ComponentDestructorFn destructor) {
        destructors()[name] = destructor;
    }

    ErrorOrVoid 
    ComponentFactory::makeComponent(std::shared_ptr<IScene> scene, EntityID ent,
        const std::string& name, const std::unordered_map<std::string, HVariant>& args) {

        if (constructors().count(name)) {
            constructors()[name](scene, ent, args);
            return ErrorOrVoid();
        }

        StringError err;
        err.errorMessage = "El componente solicitado no existe o no ha sido registrado para fabricar";
        return ErrorOrVoid(err);
    }

    ErrorOrVoid
    ComponentFactory::removeComponent(std::shared_ptr<IScene> scene, EntityID ent, const std::string& name) {

        if (destructors().count(name)) {
            destructors()[name](scene, ent);
            return ErrorOrVoid();
        }

        StringError err;
        err.errorMessage = "El componente solicitado no existe o no ha sido registrado para fabricar";
        return ErrorOrVoid(err);
    }

}