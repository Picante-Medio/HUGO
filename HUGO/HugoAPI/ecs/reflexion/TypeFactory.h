#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class TypeFactory {
public:
    using Creator = std::unique_ptr<HVariant>(*)(const std::string&);

    //Registra una función creadora y devuelve si se ha insertado con éxito
    static bool Register(const std::string& name, Creator creator) {
        return creators.emplace(name, creator).second;
    }

    static HVariant Create(const std::string& name, const std::string& args) {
        auto it = creators.find(name);
        if (it == creators.end()) return NullType{};
        return it->second(args);
    }

    static bool IsRegistered(const std::string& name) {
        return creators.find(name) != creators.end();
    }

private:
    static std::unordered_map<std::string, Creator> creators;
};