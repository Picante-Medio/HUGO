#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

// auxiliar TADS for each UI element

struct UIElementCache
{
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr;
    Ogre::OverlayElement* element = nullptr;
};

struct UIButtonCache : public UIElementCache
{
    //Ogre::BorderPanelOverlayElement* panel = nullptr;
    Ogre::TextAreaOverlayElement* textArea = nullptr;
};