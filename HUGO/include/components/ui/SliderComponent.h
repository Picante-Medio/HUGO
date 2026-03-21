#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(SliderComponent)
    // range
    HUGO::Float minValue = 0.0f;
    HUGO::Float maxValue = 100.0f;
    HUGO::Float currentValue = 0.0f;

    // orientation
    enum Orientation {
        HORIZONTAL,
        VERTICAL
    };
    Orientation orientation = HORIZONTAL;

    // knob name
    HUGO::String trackElementName = "";
    HUGO::String knobElementName = "";

    // materials
    HUGO::String trackMaterial = "";
    HUGO::String knobMaterial = "";

    // relative sizes
    HUGO::Float knobWidth = 0.0f;
    HUGO::Float knobHeight = 0.0f;

    // colour if we do not use any materials
    HUGO::Vector4 knobColor = HUGO::Vector4(-1.0f);
    HUGO::Vector4 trackColor = HUGO::Vector4(-1.0f);

    bool isDragging = false;

    std::function<void(HUGO::Float)> onChangeCallback = nullptr;
END_HCOMPONENT()