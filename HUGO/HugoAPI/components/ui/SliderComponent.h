#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(SliderComponent)
    // range
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, minValue, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, maxValue, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, currentValue, 0.0f);
    HUGO::Float previousValue = 0.0f;
    
    // orientation
    DEFAULT_SERIALIZED_MEMBER(HUGO::SliderOrientation, orientation, HUGO::SliderOrientation::HORIZONTAL);

    // knob name
    HUGO::String trackElementName = "";
    HUGO::String knobElementName = "";

    // materials
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, trackMaterial, "");
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, knobMaterial, "");

    // relative sizes
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, knobWidth, 0.05f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, knobHeight, 0.05f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, knobLeft, 0.05f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, knobTop, 0.05f);

    HUGO::Bool isDragging = false;

    std::function<void(HUGO::Float)> onChangeCallback = nullptr;
END_HCOMPONENT()

