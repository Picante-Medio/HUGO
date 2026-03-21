#pragma once

#include "HugoError.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"
#include "UIStructs.h"
#include "UITags.h"

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
std::shared_ptr<UIElementCache> createElementCache(EntityID entity, Ogre::Overlay* o, 
    Ogre::OverlayContainer* c, Ogre::OverlayElement* e = nullptr);
std::shared_ptr<UIElementCache> createElementCache(EntityID entity, const HUGO::String& overlayName,
    const HUGO::String& containerName, const HUGO::String& elementName = "");
UIElementCache* getCache(EntityID entity);

//====================================================
// HELPERS
//====================================================
//helper to calculate if the mouse cursor is above a panel
bool isMouseOverPanel(const PanelComponent& panel, HUGO::Float mouseX, HUGO::Float mouseY);
//====================================================
// SYNCHRONIZATION
//====================================================

void syncPanel(const PanelComponent& p);
void syncText(const TextComponent& t);
void syncButton(const ButtonComponent& b);
void syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr);
void syncSlider(const PanelComponent& p, const SliderComponent& s);
// void syncPanel(const PanelComponent& p, const PanelDirtyTag& tag);
// void syncText(const PanelComponent& p, const TextComponent& t, const TextDirtyTag& tag);
// void syncButton(const ButtonComponent& b, const ButtonDirtyTag& tag);
// void syncProgressBar(const PanelComponent& p, const ProgressBarComponent& pr, const TextDirtyTag& tag);

//====================================================
// PROCESSING
//====================================================
void processVisibility();
void processPanelChanges();
void processTextChanges();
void processButtonChanges();
void processProgressBarChanges();
void processInput();
void processSliderInput();
void processSliderChanges();
ErrorOrVoid processComponentCreations();
//====================================================
// CACHE
//====================================================

void clearEntityCache(EntityID entity);
void clearAllCaches();

//====================================================
// TEST UI
//====================================================
EntityID createPanel(const HUGO::String& overlayName);
EntityID createText(const HUGO::String& overlayName);
EntityID createButton();
EntityID createProgressBar(const HUGO::String& overlayName);
EntityID createSlider(const HUGO::String& overlayName);
public:

virtual ~UISystem();

void init();
void onLoad();

void update(double dt) override;
void fixedUpdate(double dt) override;
END_HSYSTEM()