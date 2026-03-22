#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(ProgressBarComponent)
    // range
    HUGO::Float minValue = 0.0f;
    HUGO::Float maxValue = 100.0f;
    HUGO::Float currentValue = 0.0f;

    // type of the bar
    enum FillMode {
        HORIZONTAL_LEFT_TO_RIGHT,
        HORIZONTAL_RIGHT_TO_LEFT,
        VERTICAL_BOTTOM_TO_TOP, 
        VERTICAL_TOP_TO_BOTTOM,
    };
    FillMode fillMode = FillMode::HORIZONTAL_LEFT_TO_RIGHT;

    // name of the fill element
    HUGO::String fillElementName = "";

    // relative width and height
    HUGO::Float fillWidth = 0.0f;
    HUGO::Float fillHeight = 0.0f;
    
    // material
    HUGO::String fillMaterial = "";
END_HCOMPONENT()