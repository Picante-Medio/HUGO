#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

HCOMPONENT(ButtonComponent)
    // names
    HUGO::String overlayName = "";            // overlay in Ogre
    HUGO::String containerName = "button";    // root container element
    HUGO::String textAreaName = "buttonText"; // text area element

    // state
    bool isVisible = false;
    bool isEnabled = true;
    bool isPressed = false;
    bool isHovered = false;
    HUGO::Int zOrder = 20;

    // position and size
    Ogre::GuiMetricsMode metricsMode = Ogre::GuiMetricsMode::GMM_RELATIVE;
    HUGO::Float left = 0.0f;
    HUGO::Float top = 0.0f;
    HUGO::Float width = 0.2f;
    HUGO::Float height = 0.1f;

    // appearance - normal state
    HUGO::String normalMaterial = "Button/Normal";
    HUGO::String hoverMaterial = "Button/Hover";
    HUGO::String pressedMaterial = "Button/Pressed";
    HUGO::String disabledMaterial = "Button/Disabled";

    // text properties
    HUGO::String text = "Button";
    HUGO::String fontName = "SdkTrays/Value";
    HUGO::Float textSize = 0.03f;
    Ogre::TextAreaOverlayElement::Alignment textAlignment = Ogre::TextAreaOverlayElement::Alignment::Center;
END_HCOMPONENT()
