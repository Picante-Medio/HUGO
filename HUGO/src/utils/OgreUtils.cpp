#include "HugoMath.h"
#include "OgreUtils.h"

#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreOverlayManager.h"


//====================================================
// CONVERSIONS FOR MATH TYPES
//====================================================
Ogre::Vector3 OgreUtils::vector3ToOgre(const HUGO::Vector3 &vec3)
{
    return Ogre::Vector3(vec3.x, vec3.y, vec3.z);
}

Ogre::Quaternion OgreUtils::quaternionToOgre(const HUGO::Quaternion &q)
{
    return Ogre::Quaternion(q.w, q.x, q.y, q.z);
}

Ogre::Radian OgreUtils::radianToOgre(const HUGO::Radian &radian)
{
    return Ogre::Radian(radian.rad);
}
Ogre::Degree OgreUtils::degreeToOgre(const HUGO::Degree &degree)
{
    return Ogre::Degree(degree.deg);
}

//====================================================
// CONVERSIONS FOR UI TYPES
//====================================================
Ogre::GuiMetricsMode OgreUtils::toOgre(HUGO::UIMetricsMode mode)
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

Ogre::GuiHorizontalAlignment OgreUtils::toOgre(HUGO::UIHorizontalAlignment align)
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

Ogre::GuiVerticalAlignment OgreUtils::toOgre(HUGO::UIVerticalAlignment align)
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

HUGO::UIMetricsMode OgreUtils::toHugo(Ogre::GuiMetricsMode mode)
{
    switch (mode) {
        case Ogre::GMM_RELATIVE: return HUGO::GMM_RELATIVE;
        case Ogre::GMM_PIXELS: return HUGO::GMM_PIXELS;
        case Ogre::GMM_RELATIVE_ASPECT_ADJUSTED: return HUGO::GMM_RELATIVE_ASPECT_ADJUSTED;
        default: return HUGO::GMM_RELATIVE;
    }
}

HUGO::UIHorizontalAlignment OgreUtils::toHugo(Ogre::GuiHorizontalAlignment align)
{
    switch (align) {
        case Ogre::GHA_LEFT: return HUGO::UHA_LEFT;
        case Ogre::GHA_CENTER: return HUGO::UHA_CENTER;
        case Ogre::GHA_RIGHT: return HUGO::UHA_RIGHT;
        default: return HUGO::UHA_LEFT;
    }
}

HUGO::UIVerticalAlignment OgreUtils::toHugo(Ogre::GuiVerticalAlignment align)
{
    switch (align) {
        case Ogre::GVA_TOP: return HUGO::UVA_TOP;
        case Ogre::GVA_CENTER: return HUGO::UVA_CENTER;
        case Ogre::GVA_BOTTOM: return HUGO::UVA_BOTTOM;
        default: return HUGO::UVA_TOP;
    }
}

ErrorOrType<Ogre::Overlay*> OgreOverlayUtils::getOverlay(const HUGO::String& name)
{
    StringError e;
    if (name.empty()) {
        e.errorMessage = "Overlay name is empty";
        return ErrorOrType<Ogre::Overlay*>(e);
    }

    Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().getByName(name);
    if (!overlay) {
        e.errorMessage = "Cannot find overlay: " + name;
        return ErrorOrType<Ogre::Overlay*>(e);
    }
    return ErrorOrType<Ogre::Overlay*>(overlay);
}

ErrorOrType<Ogre::OverlayElement*> OgreOverlayUtils::getElement(const HUGO::String& name)
{
    StringError e;
    if (name.empty()) {
        e.errorMessage = "Element name is empty";
        return ErrorOrType<Ogre::OverlayElement*>(e);
    }

    Ogre::OverlayElement* element = Ogre::OverlayManager::getSingleton().getOverlayElement(name);
    if (!element) {
        e.errorMessage = "Cannot find element: " + name;
        return ErrorOrType<Ogre::OverlayElement*>(e);
    }
    return ErrorOrType<Ogre::OverlayElement*>(element);
}

ErrorOrType<Ogre::OverlayContainer*> OgreOverlayUtils::getContainer(const HUGO::String& name)
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
    return ErrorOrType<Ogre::OverlayContainer*>(container);
}
