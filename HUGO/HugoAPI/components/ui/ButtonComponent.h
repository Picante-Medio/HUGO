#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(ButtonComponent)
    // state
    HUGO::ButtonStateTypes state = HUGO::ButtonStateTypes::BS_UP;
    HUGO::TrayLocationTypes mTrayLoc;

    std::function<void()> onClickCallback;
    bool wasOver = false;
    
    // appearance - normal state
    DEFAULT_SERIALIZED_MEMBER(std::string, normalMaterial, "Button/Normal");
    DEFAULT_SERIALIZED_MEMBER(std::string, hoverMaterial, "Button/Hover");
    DEFAULT_SERIALIZED_MEMBER(std::string, pressedMaterial, "Button/Pressed");
    DEFAULT_SERIALIZED_MEMBER(std::string, disabledMaterial, "Button/Disabled");
END_HCOMPONENT()

