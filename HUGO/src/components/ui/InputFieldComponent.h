#pragma once
#include "Component.h"

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(InputFieldComponent)
    bool isActive = false;
    std::string originalText;

    std::function<void(const HUGO::String&)> onChangeCallback;

    HUGO::String result = "";
END_HCOMPONENT()