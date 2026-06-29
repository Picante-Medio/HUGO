#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class IType;

class TypeFactory {
public:
    using Creator = std::unique_ptr<IType>(*)(const std::string&);

    //Registra una función creadora y devuelve si se ha insertado con éxito
    static bool Register(const char* name, Creator creator) {
        return creators.emplace(name, creator).second;
    }

    static std::unique_ptr<IType> Create(const char* name, const std::string& args) {
        auto it = creators.find(name);
        if (it == creators.end()) return nullptr;
        return it->second(args);
    }

private:
    static std::unordered_map<std::string, Creator> creators;
};