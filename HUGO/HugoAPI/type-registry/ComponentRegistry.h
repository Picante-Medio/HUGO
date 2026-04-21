#pragma once

#include "UtilsAPI.h"

#include "EcsTypes.h"
#ifdef __linux__ 
#include <cstdint>
#endif

#include <vector>

namespace HUGO {

    class ComponentRegistry {

        static UTILS_API uint32_t GetIdFromString(const char* name);

    public:

        template<typename T>
        static ComponentID getComponentId() {
            static ComponentID index = GetIdFromString(T::name);
            return index;
        }

        template<typename... Ts>
        static std::vector<ComponentID> getAllComponentIds() {
            return { getComponentId<Ts>()... };
        }
    };
}