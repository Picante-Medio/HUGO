#pragma once
#include "Component.h"

#include "HugoTypes.h"
#include "HugoComponentMacros.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

HCOMPONENT(UICanvasComponent)
    // names
    HUGO::String overlayName; // overlay in Ogre
    HUGO::String containerName; // root container element

    // state
    bool isVisible;
    HUGO::Int zOrder;
    Ogre::GuiMetricsMode metricsMode; // relative/pixels

    // position and size (default full screen)
    HUGO::Float left;
    HUGO::Float top;
    HUGO::Float width;
    HUGO::Float height;

    // appearance
    HUGO::String backgroundMaterial;

    UICanvasComponent(EntityID entity, const HUGO::String& name, 
        bool visible = false, HUGO::Int order = 10, 
        Ogre::GuiMetricsMode mode = Ogre::GuiMetricsMode::GMM_RELATIVE,
        HUGO::Float left = 0.0f, HUGO::Float top =0.0f,
        HUGO::Float width = 1.0f, HUGO::Float height = 1.0f,
        const HUGO::String& material = ""): 
    Component(entity), overlayName(name), containerName(name + "Container"), 
    isVisible(false), zOrder(order), metricsMode(mode),
    left(left), top(top), width(width), height(height), backgroundMaterial(material)
    {}
END_HCOMPONENT()