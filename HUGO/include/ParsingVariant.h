#pragma once

#include <variant>
#include <string>

using Variant = std::variant<
    int,
    float,
    std::string
>;
