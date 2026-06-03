#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(ProgressBarComponent)
    // range
    DEFAULT_SERIALIZED_MEMBER(float, minValue, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, maxValue, 100.0f);
    DEFAULT_SERIALIZED_MEMBER(float, currentValue, 0.0f);

    // type of the bar
    DEFAULT_SERIALIZED_MEMBER(HUGO::ProgressBarFillMode, fillMode, HUGO::ProgressBarFillMode::HORIZONTAL_LEFT_TO_RIGHT);

    // name of the fill element
    DEFAULT_SERIALIZED_MEMBER(std::string, fillElementName, "");

    // material
    DEFAULT_SERIALIZED_MEMBER(std::string, fillMaterial, "");
END_HCOMPONENT()

