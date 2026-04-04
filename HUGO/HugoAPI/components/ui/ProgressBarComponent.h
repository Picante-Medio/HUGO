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
    HUGO::ProgressBarFillMode fillMode = HUGO::ProgressBarFillMode::HORIZONTAL_LEFT_TO_RIGHT;

    // name of the fill element
    HUGO::String fillElementName = "";

    // relative width and height
    HUGO::Float fillWidth = 0.0f;
    HUGO::Float fillHeight = 0.0f;
    
    // material
    HUGO::String fillMaterial = "";
END_HCOMPONENT()

