#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(InputFieldComponent)
    bool isActive = false;
    HUGO::String originalText;

    std::function<void(const HUGO::String&)> onChangeCallback;

    HUGO::String result = "";
END_HCOMPONENT()