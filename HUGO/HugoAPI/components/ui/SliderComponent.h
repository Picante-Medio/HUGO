#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(SliderComponent)
    // range
    DEFAULT_SERIALIZED_MEMBER(float, minValue, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, maxValue, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(float, currentValue, 0.0f);
    float previousValue = 0.0f;
    
    // orientation
    DEFAULT_SERIALIZED_MEMBER(HUGO::SliderOrientation, orientation, HUGO::SliderOrientation::HORIZONTAL);

    // knob name
    DEFAULT_SERIALIZED_MEMBER(std::string, trackElementName, "");
    DEFAULT_SERIALIZED_MEMBER(std::string, knobElementName, "");

    // materials
    DEFAULT_SERIALIZED_MEMBER(std::string, trackMaterial, "");
    DEFAULT_SERIALIZED_MEMBER(std::string, knobMaterial, "");

    // relative sizes
    DEFAULT_SERIALIZED_MEMBER(float, knobWidth, 0.05f);
    DEFAULT_SERIALIZED_MEMBER(float, knobHeight, 0.05f);
    DEFAULT_SERIALIZED_MEMBER(float, knobLeft, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, knobTop, 0.0f);

    bool isDragging = false;

    std::function<void(float)> onChangeCallback = nullptr;
END_HCOMPONENT()

