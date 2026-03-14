#include "Scene.h"
#include "UISystem.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"

#include <Ogre.h>
#include <OgreBorderPanelOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

#include "OgreModule.h"

UISystem::UISystem(std::shared_ptr<Scene> scene)
    : System(scene), renderWindow(OgreModule::Instance().getOgreRenderWindow())
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
        cache->panel = containerResult.getValue();
    }
    return cache;
}

std::shared_ptr<TextCache> UISystem::createTextCache(const TextComponent &t)
{
    std::shared_ptr<TextCache> cache = std::make_shared<TextCache>();
    
    // get Ogre objects
    ErrorOrType<Ogre::Overlay*> overlayResult = getOverlay(t.overlayName);
    if (!overlayResult.isError()) {
        cache->overlay = overlayResult.getValue();
    }
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(t.containerName);
    if (!containerResult.isError()) {
        cache->container = containerResult.getValue();
    }
    
    // get text area using getElement + dynamic_cast
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(t.textAreaName);
    if (!elementResult.isError()) {
        Ogre::OverlayElement* element = elementResult.getValue();
        Ogre::TextAreaOverlayElement* textArea = dynamic_cast<Ogre::TextAreaOverlayElement*>(element);
        
        // store current state from component
        if (textArea != nullptr)
            cache->textArea = textArea;
    }
    
    return cache;
}

std::shared_ptr<ButtonCache> UISystem::createButtonCache(const ButtonComponent &b)
{
    std::shared_ptr<ButtonCache> cache = std::make_shared<ButtonCache>();
    
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(b.containerName);
    if (!containerResult.isError()) {
        cache->container = containerResult.getValue();
    }
    
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(b.textAreaName);
    if (!elementResult.isError()) {
        Ogre::OverlayElement* element = elementResult.getValue();
        Ogre::TextAreaOverlayElement* textArea = dynamic_cast<Ogre::TextAreaOverlayElement*>(element);
        cache->element = element;
        // store current state from component
        if (textArea != nullptr) {
            cache->textArea = textArea;
            if(b.width > 0){
                element->setWidth(b.width);
            }
            textArea->setCaption(b.text);
        }

        Ogre::BorderPanelOverlayElement* mBP = (Ogre::BorderPanelOverlayElement*)element; 
        mBP->setBorderMaterialName("SdkTrays/Button/Up");
        mBP->setMaterialName("SdkTrays/Button/Up");
    }
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
    if (cache == nullptr || cache->overlay == nullptr || cache->panel == nullptr) return;
    
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
        if (it != panelCache.end() && it->second && it->second->overlay && it->second->panel) {
            std::shared_ptr<PanelCache> cache = it->second;
            
            // apply visibility to Ogre
            if (panel.isVisible) {
                cache->overlay->show();
                cache->panel->show();
            } else {
                cache->overlay->hide();
                cache->panel->hide();
            }
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
        if (it != textCache.end() && it->second && it->second->overlay && it->second->container && it->second->textArea) {
            std::shared_ptr<TextCache> cache = it->second;
            
            // apply visibility to Ogre
            if (text.isVisible) {
                cache->overlay->show();
                cache->container->show();
                cache->textArea->show();
            } else {
                cache->overlay->hide();
                cache->container->hide();
                cache->textArea->hide();
            }
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
    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(ent);

    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container"; 

    // 1. get overlay
    Ogre::Overlay* overlay = om.getByName(panel.overlayName);
    if (!overlay) return;

    panel.zOrder = overlay->getZOrder();

    // 2. get container
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(panel.containerName);
    if (containerResult.isError()) return;

    Ogre::OverlayContainer* container = containerResult.getValue();
    container->setMaterialName("UI/Test");
    panel.material = container->getMaterialName();
    panel.metricsMode = container->getMetricsMode();
    panel.left = container->getLeft();
    panel.top = container->getTop();
    panel.width = container->getWidth();
    panel.height = container->getHeight();
    panel.alignmentX = container->getHorizontalAlignment();
    panel.alignmentY = container->getVerticalAlignment();
    
    // visibility
    panel.isVisible = true;
    scene->addComponent<VisibilityDirtyTag>(ent);

    panelCache[ent] = createPanelCache(panel);
}

void UISystem::createTestText()
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
    

    // 3. get text area
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(text.textAreaName);
    if (!elementResult.isError()) {

        Ogre::TextAreaOverlayElement* area =
            dynamic_cast<Ogre::TextAreaOverlayElement*>(elementResult.getValue());

        if (area) {
            text.text = area->getCaption();
            text.characterHeight = area->getCharHeight();
            
            text.metricsMode = area->getMetricsMode();
            text.left = area->getLeft();
            text.top = area->getTop();
            text.width = area->getWidth();
            text.height = area->getHeight();


            Ogre::ColourValue c = area->getColour();
            Ogre::ColourValue ct = area->getColourTop();
            Ogre::ColourValue cb = area->getColourBottom();
            text.colour = {c.r, c.g, c.b, c.a};
            text.colourTop = {ct.r, ct.g, ct.b, ct.a};
            text.colourBottom = {cb.r, cb.g, cb.b, cb.a};
            text.spaceWidth = area->getSpaceWidth();
            text.alignmentX = area->getHorizontalAlignment();
            text.alignmentY = area->getVerticalAlignment();
        }
    }
    
    // EntityID buto = scene->createEntity();
    // ButtonComponent& button = scene->addAndGetComponent<ButtonComponent>(buto);
    // button.overlayName = "testCanvas/SimpleTest";
    // button.containerName = button.overlayName + "/Container";
    // button.textAreaName = button.containerName + "/Button";
    // buttonCache[buto] = createButtonCache(button);

    // visibility
    text.isVisible = true;
    scene->addComponent<VisibilityDirtyTag>(ent);

    // cache correcto
    textCache[ent] = createTextCache(text);
}
