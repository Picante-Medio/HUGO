#pragma once

#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include <OgreOverlay.h>

HCOMPONENT(overlayComponent)
    // names
    HUGO::String overlayName=""; // overlay in Ogre
    // state
    bool isVisible = false;
END_HCOMPONENT()