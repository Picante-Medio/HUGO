#include "ComponentRegistry.h"

#include <unordered_map>
#include <string>

namespace HUGO {

    uint32_t 
    ComponentRegistry::GetIdFromString(const char* name){
        
        static std::unordered_map<std::string, uint32_t> idMap = std::unordered_map<std::string, uint32_t>();
        static uint32_t idCounter = 0;

        std::string sName(name);

        if (!idMap.count(sName)) {
            idMap[sName] = idCounter++;
        }

        return idMap[sName];
    }

}