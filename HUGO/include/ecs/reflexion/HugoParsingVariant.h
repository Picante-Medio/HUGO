#pragma once

#include <variant>
#include <string>

using HVariant = std::variant<
    int,
    float,
    std::string
>;
