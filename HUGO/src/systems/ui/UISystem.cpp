#include "Scene.h"
#include "UISystem.h"
#include "PanelComponent.h"
#include "TextComponent.h"

#include <Ogre.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

UISystem::UISystem(std::shared_ptr<Scene> scene, Ogre::RenderWindow* window)
    : System(scene), renderWindow(window)
{
}

UISystem::~UISystem()
{
    clearAllCaches();
}

ErrorOrType<Ogre::Overlay*> UISystem::getOverlay(const HUGO::String& name)
{
    StringError err;
    if (name.empty()){
        err.errorMessage = "Overlay name is empty";
        return ErrorOrType<Ogre::Overlay*>(err);
    }
    // the manager is initialized the first time we use 
    Ogre::OverlayManager& mng = Ogre::OverlayManager::getSingleton();
    Ogre::Overlay* o = mng.getByName(name);
    if (o==nullptr){
        err.errorMessage = "Cannot find overlay: " + name;
        return ErrorOrType<Ogre::Overlay*>(err);
    }
    else return ErrorOrType<Ogre::Overlay*>(o);
}

ErrorOrType<Ogre::OverlayElement*> UISystem::getElement(const HUGO::String& name)
{
    StringError err;
    if (name.empty()) {
        err.errorMessage = "Element name is empty";
        return ErrorOrType<Ogre::OverlayElement*>(err);
    }
    
    // get from Ogre, returns nullptr if not found
    Ogre::OverlayManager& mng = Ogre::OverlayManager::getSingleton();
    Ogre::OverlayElement* e = mng.getOverlayElement(name);
    
    // check if found
    if (e == nullptr) {
        err.errorMessage = "Cannot find element: " + name;
        return ErrorOrType<Ogre::OverlayElement*>(err);
    }
    else return ErrorOrType<Ogre::OverlayElement*>(e);
}

ErrorOrType<Ogre::OverlayContainer*> UISystem::getContainer(const HUGO::String& name)
{
    StringError err;
    
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(name);
    if (elementResult.isError()) {
        return ErrorOrType<Ogre::OverlayContainer*>(elementResult.getError());
    }
    
    Ogre::OverlayContainer* container = dynamic_cast<Ogre::OverlayContainer*>(elementResult.getValue());
    
    if (container == nullptr) {
        err.errorMessage = "Element exists but is not a container: " + name;
        return ErrorOrType<Ogre::OverlayContainer*>(err);
    }
    
    return ErrorOrType<Ogre::OverlayContainer*>(container);
}

// helper method to create canvas cache from component
std::shared_ptr<PanelCache> UISystem::createPanelCache(const PanelComponent& p)
{
    std::shared_ptr<PanelCache> cache = std::make_shared<PanelCache>();
    
    // get Ogre objects
    ErrorOrType<Ogre::Overlay*> overlayResult = getOverlay(p.overlayName);
    if (!overlayResult.isError()) {
        cache->overlay = overlayResult.getValue();
    }
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(p.containerName);
    if (!containerResult.isError()) {
        cache->container = containerResult.getValue();
    }
    
    // store current state from component
    cache->lastVisibleState = p.isVisible;
    cache->lastZOrder = p.zOrder;
    cache->lastLeft = p.left;
    cache->lastTop = p.top;
    cache->lastWidth = p.width;
    cache->lastHeight = p.height;
    cache->lastMaterial = p.backgroundMaterial;
    
    return cache;
}

std::shared_ptr<TextCache> UISystem::createTextCache(const TextComponent &t)
{
    std::shared_ptr<TextCache> cache = std::make_shared<TextCache>();
    
    // get Ogre objects
    ErrorOrType<Ogre::Overlay*> overlayResult = getOverlay(t.overlayName);
    if (!overlayResult.isError()) {
        cache->overlay = overlayResult.getValue();
        cache->lastZOrder = cache->overlay->getZOrder();
    }
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(t.containerName);
    if (!containerResult.isError()) {
        cache->container = containerResult.getValue();
        cache->lastPanelMaterial = cache->container->getMaterialName();
    }
    
    // get text area using getElement + dynamic_cast
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(t.textAreaName);
    if (!elementResult.isError()) {
        Ogre::OverlayElement* element = elementResult.getValue();
        Ogre::TextAreaOverlayElement* textArea = dynamic_cast<Ogre::TextAreaOverlayElement*>(element);
        
        // store current state from component
        if (textArea != nullptr) {
            cache->textArea = textArea;
            
            cache->lastText = textArea->getCaption();
            cache->lastFontName = textArea->getFontName();
            cache->lastCharHeight = textArea->getCharHeight();
            
            Ogre::ColourValue c = textArea->getColour();
            Ogre::ColourValue ct = textArea->getColourTop();
            Ogre::ColourValue cb = textArea->getColourBottom();

            cache->lastColour = {c.r, c.g, c.b, c.a};
            cache->lastColourTop = {ct.r, ct.g, ct.b, ct.a};
            cache->lastColourBottom = {cb.r, cb.g, cb.b, cb.a};

            cache->lastAlignment = textArea->getAlignment();
            cache->lastLeft = textArea->getLeft();
            cache->lastTop = textArea->getTop();
            cache->lastWidth = textArea->getWidth();
            cache->lastHeight = textArea->getHeight();
            cache->lastMetricsMode = textArea->getMetricsMode();
            cache->lastSpaceWidth = textArea->getSpaceWidth();
        }
    }
    
    cache->lastVisibleState = t.isVisible;
    
    return cache;
}

// SYNCHRONIZATION
void UISystem::syncPanel(const PanelComponent& p, const PanelDirtyTag& t)
{
    // try to emplace - if it exists, get iterator; else, creates it
    auto [it, inserted] = panelCache.emplace(p.entity, nullptr);
    
    // it does not exist, so we create one
    if (inserted)
        it->second = createPanelCache(p);
    
    std::shared_ptr<PanelCache>& cache = it->second;
    if (cache == nullptr || cache->overlay == nullptr || cache->container == nullptr) return;
    
    //sync according to the fields


    // // Z-order
    // if (t.flags & HUGO::PanelDirtyFlags::Z_ORDER){
    //     cache->overlay->setZOrder(p.zOrder);
    //     cache->lastZOrder = p.zOrder;
    // }

    // // metrics mode
    // if (t.flags & HUGO::PanelDirtyFlags::METRICS_MODE){
    //     cache->container->setMetricsMode(p.metricsMode);
    // }
    
    // // position
    // if (t.flags & HUGO::PanelDirtyFlags::POSITION){
    //     // left
    //     cache->container->setLeft(p.left);
    //     cache->lastLeft = p.left;
    //     // top
    //     cache->container->setTop(p.top);
    //     cache->lastTop = p.top;
    // }

    // // size
    // if (t.flags & HUGO::PanelDirtyFlags::SIZE){
    //     // width
    //     cache->container->setWidth(p.width);
    //     cache->lastWidth = p.width;
    //     // height
    //     cache->container->setHeight(p.height);
    //     cache->lastHeight = p.height;
    // }

    // // material
    // if (t.flags & HUGO::PanelDirtyFlags::MATERIAL) {
    //     cache->container->setMaterialName(p.backgroundMaterial);
    //     cache->lastMaterial = p.backgroundMaterial;
    // }
}

void UISystem::syncText(const TextComponent &c, const TextDirtyTag &t)
{
    // // try to emplace - if it exists, get iterator; else, creates it
    auto [it, inserted] = textCache.emplace(c.entity, nullptr);
    
    // it does not exist, so we create one
    if (inserted) 
        it->second = createTextCache(c);
    
    std::shared_ptr<TextCache>& cache = it->second;
    if (cache == nullptr || cache->overlay == nullptr 
        || cache->container == nullptr || cache->textArea == nullptr) return;
    
    //sync according to the fields

    // // Z-order
    // if (t.flags & HUGO::Z_ORDER){
    //     cache->overlay->setZOrder(p.zOrder);
    //     cache->lastZOrder = p.zOrder;
    // }

    // // metrics mode
    // if (t.flags & HUGO::PanelDirtyFlags::METRICS_MODE){
    //     cache->container->setMetricsMode(p.metricsMode);
    // }
    
    // // position
    // if (t.flags & HUGO::PanelDirtyFlags::POSITION){
    //     // left
    //     cache->container->setLeft(p.left);
    //     cache->lastLeft = p.left;
    //     // top
    //     cache->container->setTop(p.top);
    //     cache->lastTop = p.top;
    // }

    // // size
    // if (t.flags & HUGO::PanelDirtyFlags::SIZE){
    //     // width
    //     cache->container->setWidth(p.width);
    //     cache->lastWidth = p.width;
    //     // height
    //     cache->container->setHeight(p.height);
    //     cache->lastHeight = p.height;
    // }

    // // material
    // if (t.flags & HUGO::PanelDirtyFlags::MATERIAL) {
    //     cache->container->setMaterialName(p.backgroundMaterial);
    //     cache->lastMaterial = p.backgroundMaterial;
    // }
}

void UISystem::processVisibility()
{
    // PANEL
    for (auto& pack : scene->query<PanelComponent, VisibilityDirtyTag>()) {
        PanelComponent& panel = pack.get<PanelComponent&>();
        EntityID entity = panel.entity;
        
        // get cache for this entity
        auto it = panelCache.find(entity);
        if (it != panelCache.end() && it->second && it->second->overlay) {
            std::shared_ptr<PanelCache> cache = it->second;
            
            // apply visibility to Ogre
            if (panel.isVisible) {
                cache->overlay->show();
                if (cache->container) {
                    cache->container->show();
                }
            } else {
                cache->overlay->hide();
                if (cache->container) {
                    cache->container->hide();
                }
            }
            
            // update cache
            cache->lastVisibleState = panel.isVisible;
        }
        
        // remove the dirty tag
        scene->removeComponent<VisibilityDirtyTag>(entity);
    }
    // TEXT
    for (auto& pack : scene->query<TextComponent, VisibilityDirtyTag>()) {
        TextComponent& text = pack.get<TextComponent&>();
        EntityID entity = text.entity;
        
        // get cache for this entity
        auto it = textCache.find(entity);
        if (it != textCache.end() && it->second && it->second->overlay) {
            std::shared_ptr<TextCache> cache = it->second;
            
            // apply visibility to Ogre
            if (text.isVisible) {
                cache->overlay->show();
                if (cache->container) {
                    cache->container->show();
                    if (cache->textArea){
                        cache->textArea->show();
                    }
                }
            } else {
                cache->overlay->hide();
                if (cache->container) {
                    cache->container->hide();
                    if (cache->textArea){
                        cache->textArea->hide();
                    }
                }
            }
            
            // update cache
            cache->lastVisibleState = text.isVisible;
        }
        
        // remove the dirty tag
        scene->removeComponent<VisibilityDirtyTag>(entity);
    }
}

void UISystem::processPanelChanges()
{
    for (auto& pack : scene->query<PanelComponent, PanelDirtyTag>()) {
        PanelComponent& panel = pack.get<PanelComponent&>();
        PanelDirtyTag& tag = pack.get<PanelDirtyTag&>();
        EntityID entity = panel.entity;
        syncPanel(panel, tag);
        scene->removeComponent<PanelDirtyTag>(entity);
    }
}

void UISystem::processTextChanges()
{
    for (auto& pack : scene->query<TextComponent, TextDirtyTag>()) {
        TextComponent& text = pack.get<TextComponent&>();
        TextDirtyTag& tag = pack.get<TextDirtyTag&>();
        EntityID entity = text.entity;
        syncText(text, tag);
        scene->removeComponent<TextDirtyTag>(entity);
    }
}

void UISystem::clearEntityCache(EntityID entity) {
    panelCache.erase(entity);
}

void UISystem::clearAllCaches() {
    panelCache.clear();
}

//=============================================================================
// UPDATE
//=============================================================================

void UISystem::update(double dt)
{
    processPanelChanges();
    processTextChanges();
    processVisibility();
}

void UISystem::fixedUpdate(double dt) {}

void UISystem::createTestCanvas()
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();

    EntityID ent = scene->createEntity();
    TextComponent& text = scene->addAndGetComponent<TextComponent>(ent);

    text.overlayName = "testCanvas/SimpleTest";
    text.containerName = text.overlayName + "/Container";
    text.textAreaName = text.containerName + "/Text";

    // 1. get overlay
    Ogre::Overlay* overlay = om.getByName(text.overlayName);
    if (!overlay) return;

    text.zOrder = overlay->getZOrder();

    // 2. get container
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(text.containerName);
    if (containerResult.isError()) return;

    Ogre::OverlayContainer* container = containerResult.getValue();
    container->setMaterialName("UI/Test");
    text.panelMaterial = container->getMaterialName();
    text.metricsMode = container->getMetricsMode();
    text.left = container->getLeft();
    text.top = container->getTop();
    text.width = container->getWidth();
    text.height = container->getHeight();

    // 3. get text area
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(text.textAreaName);
    if (!elementResult.isError()) {

        Ogre::TextAreaOverlayElement* area =
            dynamic_cast<Ogre::TextAreaOverlayElement*>(elementResult.getValue());

        if (area) {
            text.text = area->getCaption();
            text.characterHeight = area->getCharHeight();

            Ogre::ColourValue c = area->getColour();
            text.colour = {c.r, c.g, c.b, c.a};

            text.spaceWidth = area->getSpaceWidth();
            area->setAlignment(Ogre::TextAreaOverlayElement::Center);
            text.alignmentX = area->getHorizontalAlignment();
            text.alignmentY = area->getVerticalAlignment();

            area->setAlignment(Ogre::TextAreaOverlayElement::Center);
            area->setLeft(0);
            area->setTop(0);
            area->setWidth(container->getWidth());
            area->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_CENTER);
            area->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
            area->setHeight(container->getHeight());
        }
    }

    // visibility
    text.isVisible = true;
    scene->addComponent<VisibilityDirtyTag>(ent);

    // cache correcto
    textCache[ent] = createTextCache(text);
}