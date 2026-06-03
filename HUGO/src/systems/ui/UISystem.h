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
        
    std::shared_ptr<UIElementCache> createElementCache(EntityID entity, const std::string& overlayName, const std::string& containerName);
    UIElementCache* getCache(EntityID entity);

    //====================================================
    // HELPERS
    //====================================================
    //helper to calculate if the mouse cursor is above a panel
    bool isMouseOver(float left, float top, float width, float height, float mouseX, float mouseY);
    bool isMouseOverPanel(const PanelComponent& panel, float mouseX, float mouseY);
    //====================================================
    // SYNCHRONIZATION
    //====================================================

    void syncPanel(const PanelComponent& p);
    void syncText(const TextComponent& t);
    bool isValidColour(const HUGO::Vector4& colour);
    std::string formatText(const std::string& text, Ogre::TextAreaOverlayElement* textArea,float maxWidth,float maxHeight);
    void syncButton(const ButtonComponent& b);
    void syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr);
    void syncSlider(const PanelComponent& p, SliderComponent& s);

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
    void hideCache(UIElementCache* cache, Ogre::OverlayManager& mng);
    void clearAllCaches();
public:
    virtual ~UISystem();

    void init() override;
    void onLoad() override;
    void onExit() override;

    void update(uint32_t dt) override;
    void fixedUpdate(uint32_t dt) override;
END_HSYSTEM()