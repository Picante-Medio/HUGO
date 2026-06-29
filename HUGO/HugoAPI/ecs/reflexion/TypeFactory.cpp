#pragma once

#include "TypeFactory.h"

bool TypeFactory::Register(const std::string& name, Creator creator) {
    return creators.emplace(name, creator).second;
}

HVariant TypeFactory::Create(const std::string& name, const std::string& args) {
    auto it = creators.find(name);
    if (it == creators.end()) return NullType{};
    return it->second(args);
}

bool TypeFactory::IsRegistered(const std::string& name) {
    return creators.find(name) != creators.end();
}