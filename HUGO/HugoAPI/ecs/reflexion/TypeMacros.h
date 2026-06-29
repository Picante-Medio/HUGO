#pragma once
#include "TypeFactory.h"

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#define REGISTER_TYPE(Type) \
    namespace { \
        static const bool registered_##Type = [] { \
            TypeFactory::Register(STRINGIFY(Type), &Type::Create); \
            return true; \
        }(); \
    }