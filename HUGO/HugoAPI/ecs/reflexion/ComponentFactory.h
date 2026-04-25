#pragma once

#include "UtilsAPI.h"

#include <string>
#include <unordered_map>
#include <functional>

#include "IScene.h"
#include "HugoError.h"
#include "HugoParsingVariant.h"

namespace HUGO {

    class UTILS_API ComponentFactory {
    public:
        using ComponentConstructorFn = ErrorOrVoid(*)(std::shared_ptr<IScene>, EntityID, const std::unordered_map<std::string, HVariant>&);
        using ComponentDestructorFn = ErrorOrVoid(*)(std::shared_ptr<IScene>, EntityID);

    private:
        static std::unordered_map<std::string, ComponentConstructorFn>& constructors();
        static std::unordered_map<std::string, ComponentDestructorFn>& destructors();

    public:
        static void addConstructor(const std::string& name, ComponentConstructorFn generator);
        static void addDestructor(const std::string& name, ComponentDestructorFn destructor);

        static ErrorOrVoid makeComponent(std::shared_ptr<IScene> scene, EntityID ent,
            const std::string& name, const std::unordered_map<std::string, HVariant>& args);
        static ErrorOrVoid removeComponent(std::shared_ptr<IScene> scene, EntityID ent, const std::string& name);
    };

}