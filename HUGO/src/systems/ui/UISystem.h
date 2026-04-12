#pragma once

#include "HugoError.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"
#include "UIStructs.h"

#include <unordered_map>
#include <memory>

#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreBorderPanelOverlayElement.h>


struct PanelComponent;
struct TextComponent;
struct ButtonComponent;
struct ProgressBarComponent;
struct SliderComponent;

HSYSTEM(UISystem)
private:
    EntityID consoleID;

    //====================================================
    // ENGINE
    //====================================================

    Ogre::RenderWindow* renderWindow = nullptr;
    
    //====================================================
    // UI CACHE
    //====================================================

    std::unordered_map<EntityID, std::shared_ptr<UIElementCache>> uiCache;

    //====================================================
    // CACHE CREATION
    //====================================================
        
    std::shared_ptr<UIElementCache> createElementCache(EntityID entity, const HUGO::String& overlayName, const HUGO::String& containerName);
    UIElementCache* getCache(EntityID entity);

    //====================================================
    // HELPERS
    //====================================================
    //helper to calculate if the mouse cursor is above a panel
    bool isMouseOver(HUGO::Float left, HUGO::Float top, HUGO::Float width, HUGO::Float height, HUGO::Float mouseX, HUGO::Float mouseY);
    bool isMouseOverPanel(const PanelComponent& panel, HUGO::Float mouseX, HUGO::Float mouseY);
    //====================================================
    // SYNCHRONIZATION
    //====================================================

    void syncPanel(const PanelComponent& p);
    void syncText(const TextComponent& t);
    bool isValidColour(const HUGO::Vector4& colour);
    HUGO::String formatText(const HUGO::String& text, Ogre::TextAreaOverlayElement* textArea,HUGO::Float maxWidth,HUGO::Float maxHeight);
    void syncButton(const ButtonComponent& b);
    void syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr);
    void syncSlider(const PanelComponent& p, const SliderComponent& s);

    //====================================================
    // PROCESSING
    //====================================================
    void onUpdatePanelComponent(PanelComponent& panel);
    void onUpdateTextComponent(TextComponent& t);
    void onUpdateButtonComponent(ButtonComponent& b);
    void onUpdateProgressBarComponent(ProgressBarComponent& pb);
    void onUpdateSliderComponent(SliderComponent& s);

    void onAddPanelComponent(PanelComponent& p);
    void onAddTextComponent(TextComponent& t);
    void onAddProgressBarComponent(ProgressBarComponent& pb);
    void onAddSliderComponent(SliderComponent& s);

    void processInput();
    //====================================================
    // CACHE
    //====================================================
    void clearAllCaches();
public:
    virtual ~UISystem();

    void init();
    void onLoad();

    void update(double dt) override;
    void fixedUpdate(double dt) override;
END_HSYSTEM()