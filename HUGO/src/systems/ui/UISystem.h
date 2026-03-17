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

HSYSTEM(UISystem)

private:

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
std::shared_ptr<UIElementCache> createElementCache(Ogre::Overlay* o, 
    Ogre::OverlayContainer* c, Ogre::OverlayElement* e = nullptr);
std::shared_ptr<UIElementCache> createElementCache(const HUGO::String& overlayName,
    const HUGO::String& containerName, const HUGO::String& elementName = "");
UIElementCache* getCache(EntityID entity);

//====================================================
// SYNCHRONIZATION
//====================================================

void syncPanel(const PanelComponent& p, const PanelDirtyTag& tag);
void syncText(const PanelComponent& p, const TextComponent& t, const TextDirtyTag& tag);
void syncButton(const ButtonComponent& b, const ButtonDirtyTag& tag);

//====================================================
// PROCESSING
//====================================================

void processVisibility();
void processPanelChanges();
void processTextChanges();
void processButtonChanges();

//====================================================
// CACHE
//====================================================

void clearEntityCache(EntityID entity);
void clearAllCaches();

//====================================================
// TEST UI
//====================================================
const PanelComponent& createTestCanvas();
void createTestText();
EntityID createButton(const HUGO::String& overlayName, const HUGO::String& text = "Button", float left = 0.4f, float top = 0.4f, float width = 0.2f, float height = 0.1f);

public:

virtual ~UISystem();

void init();
void onLoad();

void update(double dt) override;
void fixedUpdate(double dt) override;
END_HSYSTEM()