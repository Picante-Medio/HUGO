#include "Scene.h"
#include "UISystem.h"
#include "PanelComponent.h"
#include "UITags.h"

#include "OgreLogManager.h"
#include "OgreRoot.h"
#include "OgreSceneManager.h"
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
std::shared_ptr<UISystem::PanelCache> UISystem::createPanelCache(const PanelComponent& p)
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

// SYNCHRONIZATION
void UISystem::syncCanvas(const PanelComponent& p, const CanvasDirtyTag& t)
{
    // try to emplace - if it exists, get iterator; else, creates it
    auto [it, inserted] = panelCache.emplace(p.entity, nullptr);
    
    // it does not exist, so we create one
    if (inserted) {
        it->second = createPanelCache(p);
    }
    
    std::shared_ptr<PanelCache>& cache = it->second;
    if (cache == nullptr || cache->overlay == nullptr || cache->container == nullptr) return;
    
    // Z-order
    if (t.flags & HUGO::CanvasDirtyFlags::Z_ORDER){
        cache->overlay->setZOrder(p.zOrder);
        cache->lastZOrder = p.zOrder;
    }

    // metrics mode
    if (t.flags & HUGO::CanvasDirtyFlags::METRICS_MODE){
        cache->container->setMetricsMode(p.metricsMode);
    }
    
    // position
    if (t.flags & HUGO::CanvasDirtyFlags::POSITION){
        // left
        cache->container->setLeft(p.left);
        cache->lastLeft = p.left;
        // top
        cache->container->setTop(p.top);
        cache->lastTop = p.top;
    }

    // size
    if (t.flags & HUGO::CanvasDirtyFlags::SIZE){
        // width
        cache->container->setWidth(p.width);
        cache->lastWidth = p.width;
        // height
        cache->container->setHeight(p.height);
        cache->lastHeight = p.height;
    }

    // material
    if (t.flags & HUGO::CanvasDirtyFlags::MATERIAL) {
        cache->container->setMaterialName(p.backgroundMaterial);
        cache->lastMaterial = p.backgroundMaterial;
    }
}

void UISystem::processVisibility()
{
    // canvas
    for (auto& pack : scene->query<PanelComponent, VisibilityDirtyTag>()) {
        PanelComponent& canvas = pack.get<PanelComponent&>();
        EntityID entity = canvas.entity;
        
        // get cache for this entity
        auto it = panelCache.find(entity);
        if (it != panelCache.end() && it->second && it->second->overlay) {
            std::shared_ptr<PanelCache> cache = it->second;
            
            // apply visibility to Ogre
            if (canvas.isVisible) {
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
            cache->lastVisibleState = canvas.isVisible;
        }
        
        // remove the dirty tag
        scene->removeComponent<VisibilityDirtyTag>(entity);
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
    //canvas changes
    for (auto& pack : scene->query<PanelComponent, CanvasDirtyTag>()) {
        PanelComponent& canvas = pack.get<PanelComponent&>();
        CanvasDirtyTag& tag = pack.get<CanvasDirtyTag&>();
        EntityID entity = canvas.entity;
        syncCanvas(canvas, tag);
        scene->removeComponent<CanvasDirtyTag>(entity);
    }

    processVisibility();
}

void UISystem::fixedUpdate(double dt) {}

// CREATE TEST CANVAS
void UISystem::createTestCanvas()
{
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    EntityID ent = scene->createEntity();
    PanelComponent& canvas = scene->addAndGetComponent<PanelComponent>(ent);
    canvas.overlayName =  "testCanvas/SimpleTest";
    canvas.containerName = canvas.overlayName + "Container";
    
    // 1. first: load the .overlay script
    Ogre::Overlay* overlay = om.getByName(canvas.overlayName);

    // 2. read values from .overlay script
    canvas.zOrder = overlay->getZOrder();

    // 3. get container to read position/size
    ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(canvas.containerName);
    if (!containerResult.isError()) {
        Ogre::OverlayContainer* container = containerResult.getValue();

        // metrics mode
        canvas.metricsMode = container->getMetricsMode();

        // position and size
        canvas.left = container->getLeft();
        canvas.top = container->getTop();
        canvas.width = container->getWidth();
        canvas.height = container->getHeight();
        
        // material
        container->setMaterialName("UI/Red");
        canvas.backgroundMaterial = container->getMaterialName();

        // 4. override visibility
        canvas.isVisible = true;
        scene->addComponent<VisibilityDirtyTag>(ent);
    
        // 5. create cache with the pointers and store in canvas map
        panelCache[ent] = createPanelCache(canvas);
    }
}