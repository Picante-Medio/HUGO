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
    Ogre::OverlayContainer* panel = nullptr;
};

struct TextCache {
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr; 
    Ogre::TextAreaOverlayElement* textArea = nullptr;
};

struct ButtonCache {
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr; 
    Ogre::TextAreaOverlayElement* textArea = nullptr;
    Ogre::OverlayElement* element;
};