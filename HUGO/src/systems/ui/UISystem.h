#pragma once
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "System.h"

#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreRenderWindow.h>

class UISystem : public System
{
private:
    Ogre::RenderWindow* renderWindow = nullptr;
    Ogre::Overlay* getOverlay(const std::string& name);
public:
    UISystem(std::shared_ptr<Scene> scene);

   virtual ~UISystem() = default;

   // updates
   void update(double dt) override;
   void fixedUpdate(double dt) override;
};