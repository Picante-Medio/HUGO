#include "HugoMath.h"
#include "OgreUtils.h"

#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreOverlayManager.h"


//====================================================
// CONVERSIONS FOR MATH TYPES
//====================================================
Ogre::Vector3 HUGO::OgreUtils::vector3ToOgre(const HUGO::Vector3 &vec3)
{
    return Ogre::Vector3(vec3.x, vec3.y, vec3.z);
}

Ogre::Quaternion HUGO::OgreUtils::quaternionToOgre(const HUGO::Quaternion &q)
{
    return Ogre::Quaternion(q.w, q.x, q.y, q.z);
}

Ogre::Radian HUGO::OgreUtils::radianToOgre(const HUGO::Radian &radian)
{
    return Ogre::Radian(radian.rad);
}
Ogre::Degree HUGO::OgreUtils::degreeToOgre(const HUGO::Degree &degree)
{
    return Ogre::Degree(degree.deg);
}

//====================================================
// CONVERSIONS FOR UI TYPES
//====================================================
Ogre::GuiMetricsMode HUGO::OgreUtils::toOgre(HUGO::UIMetricsMode mode)
{
    switch (mode) {
        case HUGO::GMM_RELATIVE:
            return Ogre::GMM_RELATIVE;
        case HUGO::GMM_PIXELS:
            return Ogre::GMM_PIXELS;
        case HUGO::GMM_RELATIVE_ASPECT_ADJUSTED:
            return Ogre::GMM_RELATIVE_ASPECT_ADJUSTED;
        default:
            return Ogre::GMM_RELATIVE;
    }
}

Ogre::GuiHorizontalAlignment HUGO::OgreUtils::toOgre(HUGO::UIHorizontalAlignment align)
{
    switch (align) {
        case HUGO::UHA_LEFT:
            return Ogre::GHA_LEFT;
        case HUGO::UHA_CENTER:
            return Ogre::GHA_CENTER;
        case HUGO::UHA_RIGHT:
            return Ogre::GHA_RIGHT;
        default:
            return Ogre::GHA_LEFT;
    }
}

Ogre::GuiVerticalAlignment HUGO::OgreUtils::toOgre(HUGO::UIVerticalAlignment align)
{
    switch (align) {
        case HUGO::UVA_TOP:
            return Ogre::GVA_TOP;
        case HUGO::UVA_CENTER:
            return Ogre::GVA_CENTER;
        case HUGO::UVA_BOTTOM:
            return Ogre::GVA_BOTTOM;
        default:
            return Ogre::GVA_TOP;
    }
}

HUGO::UIMetricsMode HUGO::OgreUtils::toHugo(Ogre::GuiMetricsMode mode)
{
    switch (mode) {
        case Ogre::GMM_RELATIVE: return HUGO::GMM_RELATIVE;
        case Ogre::GMM_PIXELS: return HUGO::GMM_PIXELS;
        case Ogre::GMM_RELATIVE_ASPECT_ADJUSTED: return HUGO::GMM_RELATIVE_ASPECT_ADJUSTED;
        default: return HUGO::GMM_RELATIVE;
    }
}

HUGO::UIHorizontalAlignment HUGO::OgreUtils::toHugo(Ogre::GuiHorizontalAlignment align)
{
    switch (align) {
        case Ogre::GHA_LEFT: return HUGO::UHA_LEFT;
        case Ogre::GHA_CENTER: return HUGO::UHA_CENTER;
        case Ogre::GHA_RIGHT: return HUGO::UHA_RIGHT;
        default: return HUGO::UHA_LEFT;
    }
}

HUGO::UIVerticalAlignment HUGO::OgreUtils::toHugo(Ogre::GuiVerticalAlignment align)
{
    switch (align) {
        case Ogre::GVA_TOP: return HUGO::UVA_TOP;
        case Ogre::GVA_CENTER: return HUGO::UVA_CENTER;
        case Ogre::GVA_BOTTOM: return HUGO::UVA_BOTTOM;
        default: return HUGO::UVA_TOP;
    }
}

ErrorOrType<Ogre::Overlay*> HUGO::OgreOverlayUtils::getOverlay(const HUGO::String& name)
{
    StringError e;
    if (name.empty()) {
        e.errorMessage = "Overlay name is empty";
        return ErrorOrType<Ogre::Overlay*>(e);
    }
    
    Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName(name);
    // if it is not created yet, just create one
    if (overlay == 0){
        overlay = Ogre::OverlayManager::getSingleton().create(name);
        if (!overlay) {
            e.errorMessage = "Cannot create overlay: " + name;
            return ErrorOrType<Ogre::Overlay*>(e);
        }
    }
    return ErrorOrType<Ogre::Overlay*>(overlay);
}
ErrorOrType<Ogre::OverlayElement*> HUGO::OgreOverlayUtils::getElement(const HUGO::String& name, bool isTextArea,
    Ogre::OverlayContainer* containerToAdd)
{
    StringError e;

    if (name.empty())
    {
        e.errorMessage = "[OgreUtils] Element name is empty";
        return ErrorOrType<Ogre::OverlayElement*>(e);
    }

    Ogre::OverlayElement* element = nullptr;

    if (!Ogre::OverlayManager::getSingleton().hasOverlayElement(name))
    {
        std::string type = isTextArea ? "TextArea" : "Panel";
        element = Ogre::OverlayManager::getSingleton().createOverlayElement(type, name);
        // only attach if we need to
        if (containerToAdd != nullptr)
        {
            if (element->getParent() == nullptr)
                containerToAdd->addChild(element);
        }
    }
    else
    {
        element = Ogre::OverlayManager::getSingleton().getOverlayElement(name);
    }

    return ErrorOrType<Ogre::OverlayElement*>(element);
}


ErrorOrType<Ogre::OverlayContainer*> HUGO::OgreOverlayUtils::getContainer(const HUGO::String& name, Ogre::Overlay* overlayToAdd)
{
    StringError e;
    ErrorOrType<Ogre::OverlayElement*> elementResult = getElement(name);
    if (elementResult.isError()){
        e.errorMessage = elementResult.getError().errorMessage;
        return ErrorOrType<Ogre::OverlayContainer*>(e);
    }

    Ogre::OverlayContainer* container = dynamic_cast<Ogre::OverlayContainer*>(elementResult.getValue());
    if (!container){
        e.errorMessage = "Element exists but is not a container: " + name;
        return ErrorOrType<Ogre::OverlayContainer*>(e);
    }
    if (overlayToAdd != nullptr && container->getParent() == nullptr) {
        overlayToAdd->add2D(container);
    }
    return ErrorOrType<Ogre::OverlayContainer*>(container);
}
