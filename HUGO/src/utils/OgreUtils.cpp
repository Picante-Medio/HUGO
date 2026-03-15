#include "HugoMath.h"
#include "OgreUtils.h"

#include "OgreOverlay.h"
#include "OgreOverlayContainer.h"
#include "OgreOverlayManager.h"
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
