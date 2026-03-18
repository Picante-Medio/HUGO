#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

// Estos includes no pueden estar aquí
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

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
