#pragma once

#include <variant>
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"

using HVariant = std::variant<
    //---------------------MANDATORY CONCRETE TYPES FOR PARSING-------------
    HUGO::Int, //INT
    float, //FLOAT
    double, //DOUBLE N
    std::string, //STRING
    HUGO::Vector4, //N
    HUGO::Vector3, //VECTOR_3
    HUGO::Vector2, //VECTOR_2 N
    HUGO::Quaternion, //VECTOR_4 N
    bool, //BOOL
    
    //----------------------VALUES THAT CAN BE REPRESENTED AS FLOATS-------------
    HUGO::Degree, //FLOAT
    HUGO::Radian, //FLOAT

    //-------------------SPECIAL TREATMENT-----------------
    HUGO::ColliderInfo, //COLLIDER_TYPE
    

    //----------------------ENUMS THAT CAN BE REPRESENTED AS INTS----------------
    HUGO::BodyType, //INT
    HUGO::LightType, //INT
    HUGO::UIMetricsMode, //INT
    HUGO::ProjectionType, //INT
    HUGO::ProgressBarFillMode, //INT
    HUGO::SliderOrientation, //INT
    HUGO::AudioTypes //INT
>;
