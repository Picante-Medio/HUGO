#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreRenderWindow.h>

// auxiliar TADS for each UI element
struct PanelCache {
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr;
    bool lastVisibleState = false;
    HUGO::Int lastZOrder = -1;
    HUGO::Float lastLeft = -1.0f;
    HUGO::Float lastTop = -1.0f;
    HUGO::Float lastWidth = -1.0f;
    HUGO::Float lastHeight = -1.0f;
    HUGO::String lastMaterial = "";
};

struct TextCache {
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr; 
    Ogre::TextAreaOverlayElement* textArea = nullptr;
    bool lastVisibleState = false;
    HUGO::Int lastZOrder = -1;
    Ogre::GuiMetricsMode lastMetricsMode = Ogre::GuiMetricsMode::GMM_RELATIVE;
    HUGO::String lastText = "";
    HUGO::String lastFontName = "";
    HUGO::Float lastCharHeight = -1.0f;
    HUGO::Vector4 lastColour = HUGO::Vector4(-1.0f);
    HUGO::Vector4 lastColourTop = HUGO::Vector4(-1.0f);
    HUGO::Vector4 lastColourBottom = HUGO::Vector4(-1.0f);
    Ogre::TextAreaOverlayElement::Alignment lastAlignment = Ogre::TextAreaOverlayElement::Alignment::Left;
    HUGO::Float lastLeft = -1.0f;
    HUGO::Float lastTop = -1.0f;
    HUGO::Float lastWidth = -1.0f;
    HUGO::Float lastHeight = -1.0f;
    HUGO::Float lastSpaceWidth = -1.0f;
    HUGO::String lastPanelMaterial = "";
};