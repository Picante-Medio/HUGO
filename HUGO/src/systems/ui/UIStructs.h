#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgrePanelOverlayElement.h>

#include <vector>
// auxiliar TADS for each UI element

struct UIElementCache
{
    Ogre::Overlay* overlay = nullptr;
    Ogre::OverlayContainer* container = nullptr;
    Ogre::OverlayElement* element = nullptr;
    virtual ~UIElementCache() = default;
    virtual void setVisible(bool visible);
};

struct UISliderCache : public UIElementCache
{
    Ogre::OverlayContainer* knob = nullptr;
    void setVisible(bool visible) override;
};

namespace HUGO {
    struct Rect {
        Float left =0.0f;
        Float top = 0.0f;
        Float width = 1.0f;
        Float height= 1.0f;
    };
}
