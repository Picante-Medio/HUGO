#pragma once
#include "OgreOverlay.h"
#include "OgreLight.h"
#include "HugoError.h"
#include <OgreOverlayContainer.h>
#include <OgreTextAreaOverlayElement.h>

namespace HUGO {
    struct Vector3;
    struct Quaternion;
    struct Radian;
    struct Degree;
    enum LightType;

    struct OgreUtils {
        // Conversions for math types
        static Ogre::Vector3 vector3ToOgre(const HUGO::Vector3& vec3);
        static Ogre::Quaternion quaternionToOgre(const HUGO::Quaternion& q);
        static Ogre::Radian radianToOgre(const HUGO::Radian& radian);
        static Ogre::Degree degreeToOgre(const HUGO::Degree& degree);
        
        // conversions for UI types
        static Ogre::GuiMetricsMode toOgre(HUGO::UIMetricsMode mode);
        static Ogre::GuiHorizontalAlignment toOgre(HUGO::UIHorizontalAlignment align);
        static Ogre::GuiVerticalAlignment toOgre(HUGO::UIVerticalAlignment align);

        static HUGO::UIMetricsMode toHugo(Ogre::GuiMetricsMode mode);
        static HUGO::UIHorizontalAlignment toHugo(Ogre::GuiHorizontalAlignment align);
        static HUGO::UIVerticalAlignment toHugo(Ogre::GuiVerticalAlignment align);

        //Conversions for LightTypes
        static Ogre::Light::LightTypes toOgre(HUGO::LightType type);
    };

    struct OgreOverlayUtils{
        static HUGO::ErrorOrType<Ogre::Overlay*> getOverlay(const std::string& name);
        static HUGO::ErrorOrType<Ogre::OverlayElement*> getElement(const std::string& name, bool isTextArea = false, Ogre::OverlayContainer* containerToAdd = nullptr);
        static HUGO::ErrorOrType<Ogre::OverlayContainer*> getContainer(const std::string& name, Ogre::Overlay* overlayToAdd = nullptr);
    };

}

