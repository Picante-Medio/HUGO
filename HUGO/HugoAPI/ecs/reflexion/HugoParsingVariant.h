#pragma once

#include <variant>
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"

using HVariant = std::variant<
    HUGO::Int,
    HUGO::Float,
    HUGO::Double,
    HUGO::String,
    HUGO::Vector4,
    HUGO::Vector3,
    HUGO::Vector2,
    HUGO::Bool,
    HUGO::ColliderInfo,
    HUGO::BodyType,
    HUGO::Quaternion,
    HUGO::Radian,
    HUGO::Degree,
    HUGO::LightType,
    HUGO::UIMetricsMode,
    HUGO::ProjectionType,
    HUGO::ProgressBarFillMode,
    HUGO::SliderOrientation
>;
