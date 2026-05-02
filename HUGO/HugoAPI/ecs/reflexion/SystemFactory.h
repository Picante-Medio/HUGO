#pragma once
#include <unordered_map>
#include <functional>

#include "UtilsAPI.h"

#include "HugoError.h"
#include "IScene.h"
#include "System.h"

namespace HUGO {

    class UTILS_API SystemFactory {
    public:
        //Returns shared_ptr of the system because the Scene that holds it must reference it to update it.
        using SystemGeneratorFn = HUGO::ErrorOrType<std::shared_ptr<HUGO::System>>(*)(IScene&); 
    private:
        static std::unordered_map<std::string, SystemGeneratorFn>& generators();
    public:
        static void add(const std::string& name, SystemGeneratorFn generator);
        static HUGO::ErrorOrType<std::shared_ptr<HUGO::System>> makeSystem(HUGO::IScene& scene,
            const std::string& name);
    };
}