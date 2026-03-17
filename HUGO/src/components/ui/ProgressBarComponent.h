#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(ProgressBarComponent)

    // range
    float minValue = 0.0f;
    float maxValue = 100.0f;
    float currentValue = 0.0f;

    // type of the bar
    enum FillMode {
        HORIZONTAL_LEFT_TO_RIGHT,
        HORIZONTAL_RIGHT_TO_LEFT,
        VERTICAL_BOTTOM_TO_TOP, 
        VERTICAL_TOP_TO_BOTTOM,
        RADIAL_CLOCKWISE,
        RADIAL_COUNTERCLOCKWISE
    };
    FillMode fillMode = FillMode::HORIZONTAL_LEFT_TO_RIGHT;

    // name of the fill element
    HUGO::String fillElementName = "";

    // materials
    HUGO::String backgroundMaterial = "";
    HUGO::String fillMaterial = "";

    // fill color (if we dont use any material)
    HUGO::Vector4 fillColor = HUGO::Vector4(1.0f);

    // border options
    bool showBorder = true;
    float borderSize = 2.0f;
    HUGO::Vector4 borderColor = {0.0f, 0.0f, 0.0f, 1.0f};
END_HCOMPONENT()