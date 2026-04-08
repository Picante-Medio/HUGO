#pragma once

#include <variant>
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"

using HVariant = std::variant<
    HUGO::Int,
    HUGO::Float,
    HUGO::String,
    HUGO::Vector3,
    HUGO::Vector2,
    HUGO::Quaternion,
    HUGO::Bool,
    HUGO::ColliderInfo
>;
