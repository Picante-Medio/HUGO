#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(ButtonComponent)
    // state
    HUGO::ButtonStateTypes state = HUGO::ButtonStateTypes::BS_UP;
    HUGO::TrayLocationTypes mTrayLoc;

    // appearance - normal state
    HUGO::String normalMaterial = "Button/Normal";
    HUGO::String hoverMaterial = "Button/Hover";
    HUGO::String pressedMaterial = "Button/Pressed";
    HUGO::String disabledMaterial = "Button/Disabled";
END_HCOMPONENT()
