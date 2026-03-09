#pragma once
#include "Component.h"

#include "HugoComponentMacros.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

#include <string>

HCOMPONENT(UICanvasComponent)
    // names
    std::string overlayName; // overlay in Ogre
    std::string containerName; // root container element

    // state
    bool isVisible;
    int zOrder;
    std::string metricsMode; // relative/pixels

    // position and size (default: full screen)
    float left   = 0.0f;
    float top    = 0.0f;
    float width  = 1.0f;
    float height = 1.0f;

    // appearance
    std::string backgroundMaterial;
    // float backgroundColor[4]; // RGBA if we dont want to add a .material script

    UICanvasComponent(EntityID entity, const std::string& name, int order = 10): Component(entity), 
                                                    overlayName(name + "Canvas"), 
                                                    containerName(name + "CanvasContainer"), 
                                                    isVisible(false), 
                                                    zOrder(order){}
END_HCOMPONENT()