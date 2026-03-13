#pragma once
#include "Component.h"

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include <OgreOverlay.h>
#include <OgreTextAreaOverlayElement.h>

HCOMPONENT(TextComponent)
    // names
    HUGO::String overlayName = "";      // overlay in Ogre
    HUGO::String containerName = "container";        // parent panel/container name
    HUGO::String textAreaName = "";     // text area element name

    // state
    bool isVisible = true;
    HUGO::Int zOrder = 10;
    Ogre::GuiMetricsMode metricsMode = Ogre::GuiMetricsMode::GMM_RELATIVE; // relative/pixels

    // text content
    HUGO::String text = "";              // text to display
    HUGO::Float characterHeight = 0.05f; // height in relative units

    // appearance
    HUGO::Vector4 colour = {1.0f, 1.0f, 1.0f, 1.0f};
    HUGO::Vector4 colourTop = {0.0f, 0.0f, 0.0f, 0.0f}; 
    HUGO::Vector4 colourBottom = {0.0f, 0.0f, 0.0f, 0.0f};

    // alignment and positioning
    Ogre::GuiHorizontalAlignment alignmentX = 
        Ogre::GuiHorizontalAlignment::GHA_LEFT;
    Ogre::GuiVerticalAlignment alignmentY = 
        Ogre::GuiVerticalAlignment::GVA_CENTER;
        
    // position and size relative to parent panel
    HUGO::Float left = 0.0f;
    HUGO::Float top = 0.0f;
    HUGO::Float width = 0.0f;
    HUGO::Float height = 0.0f;

    // OPTIONALS
    // space width
    HUGO::Float spaceWidth = 0.03f;  
    // panel material
    HUGO::String panelMaterial = "";
END_HCOMPONENT()