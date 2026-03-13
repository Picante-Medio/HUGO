#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include <OgreOverlay.h>

HCOMPONENT(PanelComponent)
    // names
    HUGO::String overlayName=""; // overlay in Ogre
    HUGO::String containerName="container"; // root container element

    // state
    bool isVisible = false;
    HUGO::Int zOrder = 10;
    Ogre::GuiMetricsMode metricsMode = Ogre::GuiMetricsMode::GMM_RELATIVE; // relative/pixels

    // position and size (default full screen)
    HUGO::Float left = 0.0f;
    HUGO::Float top = 0.0f;
    HUGO::Float width = 1.0f;
    HUGO::Float height= 1.0f;

    // appearance
    HUGO::String backgroundMaterial = "";

    // alignment and positioning
    Ogre::GuiHorizontalAlignment alignmentX = 
        Ogre::GuiHorizontalAlignment::GHA_LEFT;
    Ogre::GuiVerticalAlignment alignmentY = 
        Ogre::GuiVerticalAlignment::GVA_CENTER;

END_HCOMPONENT()
