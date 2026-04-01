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
    HUGO::SliderOrientation orientation = HUGO::SliderOrientation::HORIZONTAL;

    // knob name
    HUGO::String trackElementName = "";
    HUGO::String knobElementName = "";

    // materials
    HUGO::String trackMaterial = "";
    HUGO::String knobMaterial = "";

    // relative sizes
    HUGO::Float knobWidth = 0.0f;
    HUGO::Float knobHeight = 0.0f;
    HUGO::Float knobLeft = 0.0f;
    HUGO::Float knobTop =0.0f;

    bool isDragging = false;

    std::function<void(HUGO::Float)> onChangeCallback = nullptr;
END_HCOMPONENT()

