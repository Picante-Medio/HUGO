#pragma once

#include <variant>
#include <string>
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"


// Intenta incluir el header del usuario, si no existe no pasa nada
#if __has_include("UserTypes.h")
    #include "UserTypes.h"
#endif

struct NullType {};

#ifndef USER_TYPES 
#define EXTRA_TYPES NullType
#else
#define EXTRA_TYPES USER_TYPES, NullType
#endif

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
    HUGO::AudioTypes, //INT,
    EXTRA_TYPES
>;
