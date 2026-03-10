#include "Scene.h"
#include "UISystem.h"
#include "UICanvasComponent.h"
#include "UITags.h"

UISystem::UISystem(std::shared_ptr<Scene> scene, Ogre::RenderWindow* window)
    : System(scene), renderWindow(window)
{
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
std::shared_ptr<UISystem::CanvasCache> UISystem::createCanvasCache(const UICanvasComponent& canvas)
{
    std::shared_ptr<CanvasCache> cache = std::make_shared<CanvasCache>();
    
    // get Ogre objects
    ErrorOrType<Ogre::Overlay*> overlayResult = getOverlay(canvas.overlayName);
    if (!overlayResult.isError()) {
        cache->overlay = overlayResult.getValue();
    }
    
     ErrorOrType<Ogre::OverlayContainer*> containerResult = getContainer(canvas.containerName);
    if (!containerResult.isError()) {
        cache->container = containerResult.getValue();
    }
    
    // store current state from component
    cache->lastVisibleState = canvas.isVisible;
    cache->lastZOrder = canvas.zOrder;
    cache->lastLeft = canvas.left;
    cache->lastTop = canvas.top;
    cache->lastWidth = canvas.width;
    cache->lastHeight = canvas.height;
    cache->lastMaterial = canvas.backgroundMaterial;
    
    return cache;
}

// SYNCHRONIZATION
void UISystem::syncCanvas(const UICanvasComponent& canvas, const CanvasDirtyTag& t)
{
    // try to emplace - if it exists, get iterator; else, creates it
    auto [it, inserted] = canvasCache.emplace(canvas.entity, nullptr);
    
    // it does not exist, so we create one
    if (inserted) {
        it->second = createCanvasCache(canvas);
    }
    
    std::shared_ptr<CanvasCache>& cache = it->second;
    if (cache == nullptr || cache->overlay == nullptr) return;
    
    // Z-order
    if (t.flags & HUGO::CanvasDirtyFlags::Z_ORDER){
        cache->overlay->setZOrder(canvas.zOrder);
        cache->lastZOrder = canvas.zOrder;
    }

    // metrics mode
    if (t.flags & HUGO::CanvasDirtyFlags::METRICS_MODE){
        cache->container->setMetricsMode(canvas.metricsMode);
    }
    
    // position
    if (t.flags & HUGO::CanvasDirtyFlags::POSITION){
        // left
        cache->container->setLeft(canvas.left);
        cache->lastLeft = canvas.left;
        // top
        cache->container->setTop(canvas.top);
        cache->lastTop = canvas.top;
    }

    // size
    if (t.flags & HUGO::CanvasDirtyFlags::SIZE){
        // width
        cache->container->setWidth(canvas.width);
        cache->lastWidth = canvas.width;
        // height
        cache->container->setHeight(canvas.height);
        cache->lastHeight = canvas.height;
    }
}

void UISystem::processVisibility()
{
    // canvas
    for (auto& pack : scene->query<UICanvasComponent, VisibilityDirtyTag>()) {
        UICanvasComponent& canvas = pack.get<UICanvasComponent&>();
        EntityID entity = canvas.entity;
        
        // get cache for this entity
        auto it = canvasCache.find(entity);
        if (it != canvasCache.end() && it->second && it->second->overlay) {
            std::shared_ptr<CanvasCache> cache = it->second;
            
            // apply visibility to Ogre
            if (canvas.isVisible) {
                cache->overlay->show();
            } else {
                cache->overlay->hide();
            }
            
            // update cache
            cache->lastVisibleState = canvas.isVisible;
        }
        
        // remove the dirty tag
        scene->removeComponent<VisibilityDirtyTag>(entity);
    }
}

void UISystem::clearEntityCache(EntityID entity)
{
}

//=============================================================================
// UPDATE
//=============================================================================

void UISystem::update(double dt)
{
    //canvas changes
    for (auto& pack : scene->query<UICanvasComponent, CanvasDirtyTag>()) {
        UICanvasComponent& canvas = pack.get<UICanvasComponent&>();
        EntityID entity = canvas.entity;

        // remove the dirty tag
        scene->removeComponent<VisibilityDirtyTag>(entity);
    }
}

void UISystem::fixedUpdate(double dt) {}

// CREATE TEST CANVAS
void UISystem::createTestCanvas()
{
    EntityID ent = scene->createEntity();
    scene->addComponent<UICanvasComponent>(ent, "testCanvas/SimpleTest");
    
    for (auto& pack: scene->query<UICanvasComponent>()){
        UICanvasComponent& canvas = pack.get<UICanvasComponent&>();
        
        // 1. first, get the overlay to read its properties
        ErrorOrType<Ogre::Overlay*> overlayResult = getOverlay(canvas.overlayName);
        if (overlayResult.isError()) {
            return;
        }
        Ogre::Overlay* overlay = overlayResult.getValue();
        
        // 2. read values from .overlay script
        canvas.zOrder = overlay->getZOrder();
        
        // 3. get container to read position/size
        auto containerResult = getContainer(canvas.containerName);
        if (!containerResult.isError()) {
            Ogre::OverlayContainer* container = containerResult.getValue();
            
            // position and size
            canvas.left = container->getLeft();
            canvas.top = container->getTop();
            canvas.width = container->getWidth();
            canvas.height = container->getHeight();
            
            // metrics mode
            canvas.metricsMode = container->getMetricsMode();
            
            // material
            canvas.backgroundMaterial = container->getMaterialName();
        }
        
        // 4. override visibility
        canvas.isVisible = true;
        
        // 5. create cache with the pointers and store in canvas map
        canvasCache[ent] = createCanvasCache(canvas);
    }
}