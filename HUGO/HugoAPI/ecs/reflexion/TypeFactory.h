#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "UtilsAPI.h"

#include "HugoParsingVariant.h"

class UTILS_API TypeFactory {
public:
    using Creator = HVariant(*)(const std::string&);

    //Registra una función creadora y devuelve si se ha insertado con éxito
    static bool Register(const std::string& name, Creator creator);

    static HVariant Create(const std::string& name, const std::string& args);

    static bool IsRegistered(const std::string& name);

private:
    inline static std::unordered_map<std::string, Creator> creators = std::unordered_map<std::string, Creator>();
};