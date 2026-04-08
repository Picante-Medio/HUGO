#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(ButtonComponent)
    // state
    HUGO::ButtonStateTypes state = HUGO::ButtonStateTypes::BS_UP;
    HUGO::TrayLocationTypes mTrayLoc;

    std::function<void()> onClickCallback;
    
    // appearance - normal state
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, normalMaterial, "Button/Normal");
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, hoverMaterial, "Button/Hover");
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, pressedMaterial, "Button/Pressed");
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, disabledMaterial, "Button/Disabled");
END_HCOMPONENT()

