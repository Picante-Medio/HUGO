#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include "OgreOverlay.h"

HCOMPONENT(PanelComponent)

    HUGO::String overlayName = "";
    HUGO::String containerName = "";

    bool isVisible = true;

    HUGO::Int zOrder = 10;
    Ogre::GuiMetricsMode metricsMode = Ogre::GuiMetricsMode::GMM_RELATIVE;

    HUGO::Float left = 0;
    HUGO::Float top = 0;
    HUGO::Float width = 0;
    HUGO::Float height = 0;

    Ogre::GuiHorizontalAlignment alignmentX =
        Ogre::GuiHorizontalAlignment::GHA_LEFT;

    Ogre::GuiVerticalAlignment alignmentY =
        Ogre::GuiVerticalAlignment::GVA_CENTER;
        
    HUGO::String material = "";
END_HCOMPONENT()