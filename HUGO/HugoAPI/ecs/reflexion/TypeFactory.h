#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class IType;

class TypeFactory {
public:
    using Creator = std::unique_ptr<IType>(*)();

    static bool Register(const char* name, Creator creator) {
        return creators.emplace(name, creator).second;
    }

    static std::unique_ptr<IType> Create(const char* name) {
        auto it = creators.find(name);
        if (it == creators.end()) return nullptr;
        return it->second();
    }

private:
    static std::unordered_map<std::string, Creator> creators;
};