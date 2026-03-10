#pragma once
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "System.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreRenderWindow.h>

#include <string>
#include <unordered_map>

struct PanelComponent;
struct CanvasDirtyTag;
class UISystem : public System
{
private:

#pragma region auxiliar TADS for each UI element
    struct PanelCache {
        Ogre::Overlay* overlay = nullptr;
        Ogre::OverlayContainer* container = nullptr;
        bool lastVisibleState = false;
        HUGO::Int lastZOrder = 0;
        HUGO::Float lastLeft = 0.0f;
        HUGO::Float lastTop = 0.0f;
        HUGO::Float lastWidth = 0.0f;
        HUGO::Float lastHeight = 0.0f;
        HUGO::String lastMaterial = "";
    };
#pragma endregion

#pragma region attributes
    Ogre::RenderWindow* renderWindow;

    // one unordered_map per component type
    std::unordered_map<EntityID, std::shared_ptr<PanelCache>> panelCache;
#pragma endregion
    
#pragma region private methods
/*
    // example .overlay:
    overlay MyCanvas                     // overlay
    {
        overlay_element (MyPanel) Panel // overlayContainer
        {
        }
    }
*/

    // Ogre helpers
    ErrorOrType<Ogre::Overlay*> getOverlay(const HUGO::String& name); // gets the root overlay
    ErrorOrType<Ogre::OverlayElement*> getElement(const HUGO::String& name); // get a panel (can have children)
    ErrorOrType<Ogre::OverlayContainer*> getContainer(const HUGO::String& name); // get a specific ui element
    
    // Cache helpers
    std::shared_ptr<UISystem::PanelCache> createPanelCache(const PanelComponent& p);

    // Synchronization
    void syncCanvas(const PanelComponent& p, const CanvasDirtyTag& t);

    // show/hide
    void processVisibility();
    
    // Utility
    void clearEntityCache(EntityID entity);
    void clearAllCaches();
#pragma endregion

public:
    UISystem(std::shared_ptr<Scene> scene, Ogre::RenderWindow* window);
    virtual ~UISystem();

    void update(double dt) override;
    void fixedUpdate(double dt) override;
    
    void createTestCanvas();
};