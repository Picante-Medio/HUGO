#pragma once

#include <variant>
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"

using HVariant = std::variant<
    //---------------------OBLIGATORY CONCRETE TYPES FOR PARSING-------------
    HUGO::Int, //INT
    HUGO::Float, //FLOAT
    HUGO::Double, //DOUBLE N
    HUGO::String, //STRIRNG
    HUGO::Vector4, //N
    HUGO::Vector3, //VECTOR_3
    HUGO::Vector2, //VECTOR_2 N
    HUGO::Quaternion, //VECTOR_4 N
    HUGO::Bool, //BOOL
    
    //----------------------VALUES THAT CAN BE REPRESENTED AS FLOATS-------------
    HUGO::Degree, //FLOAT
    HUGO::Radian, //FLOAT

    //----------------------ENUMS THAT CAN BE REPRESENTED AS INTS----------------
    HUGO::ColliderInfo, //COLLIDER_TYPE
    HUGO::BodyType, //INT
    HUGO::LightType, //INT
    HUGO::UIMetricsMode, //INT
    HUGO::ProjectionType, //INT
    HUGO::ProgressBarFillMode, //INT
    HUGO::SliderOrientation //INT
>;
