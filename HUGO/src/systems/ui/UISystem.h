#pragma once
#include "HugoError.h"
#include "System.h"
#include "UIStructs.h"
#include "UITags.h"

#include <string>
#include <unordered_map>

struct PanelComponent;
struct TextComponent;
class UISystem : public System
{
private:
#pragma region attributes
    Ogre::RenderWindow* renderWindow;

    // one unordered_map per component type
    std::unordered_map<EntityID, std::shared_ptr<PanelCache>> panelCache;
    std::unordered_map<EntityID, std::shared_ptr<TextCache>> textCache;
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
    std::shared_ptr<PanelCache> createPanelCache(const PanelComponent& p);
    std::shared_ptr<TextCache> createTextCache(const TextComponent& t);

    // Synchronization
    void syncPanel(const PanelComponent& p, const PanelDirtyTag& t);
    void syncText(const TextComponent& c, const TextDirtyTag& t);
    // show/hide
    void processVisibility();
    void processPanelChanges();
    void processTextChanges();
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
    void createTestText();
};