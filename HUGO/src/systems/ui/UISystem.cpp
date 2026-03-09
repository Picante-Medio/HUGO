#include "UISystem.h"

#include "Scene.h"
#include "UICanvasComponent.h"

/**
 * @brief get a specific overlay element from OverlayManager
 *
 * @param name Name associated to that overlay
 * @return Pointer to the overlay or nullptr if it does no exist
 */
Ogre::Overlay *UISystem::getOverlay(const std::string &name)
{
    // get the OverlayManager singleton of OGRE
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
    
    // getByName() devuelve nullptr si no existe
    return overlayManager.getByName(name);
}

UISystem::UISystem(std::shared_ptr<Scene> scene, Ogre::RenderWindow *window) : System(scene), renderWindow(window)
{
}

void UISystem::update(double dt)
{
    for(auto& pack : scene->query<UICanvasComponent>()) { 
        UICanvasComponent& canvas = pack.get<UICanvasComponent&>();
                
        Ogre::Overlay* overlay = getOverlay(canvas.overlayName);
        if (!overlay) continue;
        
        // synk visibility
        if (canvas.isVisible && !overlay->isVisible()) {
            overlay->show();
        } else if (!canvas.isVisible && overlay->isVisible()) {
            overlay->hide();
        }
        
        // synk z order
        if (overlay->getZOrder() != canvas.zOrder) {
            overlay->setZOrder(canvas.zOrder);
        }
    }
}

void UISystem::fixedUpdate(double dt)
{
}
