#include "LightSystem.h"
#include "OgreModule.h"
#include "LightComponent.h"
#include "OgreUtils.h"
#include "TransformComponent.h"

void LightSystem::init(){
    ogreRoot = Ogre::Root::getSingletonPtr();
    
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    scene->onAddComponent<LightComponent>().connect(this, &LightSystem::registerLightEntity);
    scene->onUpdateComponent<LightComponent>().connect(this, &LightSystem::onUpdateLightComponent);
    scene->onRemoveComponent<LightComponent>().connect(this, &LightSystem::unregisterLightEntity);
}

LightSystem::~LightSystem(){
    //destroy all lights
    for(auto& l: lightMap){
        unregisterLightEntity(scene->getComponent<LightComponent>(l.first));
    }
    lightMap.clear();
}

void LightSystem::update(double dt){
    
}
// ==================== Register Light Entity ====================
void LightSystem::registerLightEntity(LightComponent& comp)
{
    EntityID entity = comp.entity;
    if (lightMap.find(entity) != lightMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreLight(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}

// ==================== Unregister Light Entity ====================
void LightSystem::unregisterLightEntity(LightComponent& comp)
{
    EntityID entity = comp.entity;
    auto it = lightMap.find(entity);
    if (it != lightMap.end()) {
        if(it->second.node){
            it->second.node->detachAllObjects();
            sceneManager->destroySceneNode(it->second.node);
            it->second.node = nullptr;
        }
        if(it->second.ogreLight){
            sceneManager->destroyLight(it->second.ogreLight);
            it->second.ogreLight = nullptr;
        }
        lightMap.erase(it);
    }
}

// ==================== Create Ogre Light ====================
ErrorOrVoid LightSystem::createOgreLight(EntityID entity)
{
    LightInfo res;
    
    // Get the transformComponent and LightComponentof the light to set the initial configuration
    const TransformComponent* transformComp = nullptr;
    const LightComponent* lightComp = nullptr;

    try{
        transformComp = &scene->getComponent<TransformComponent>(entity);
        lightComp = &scene->getComponent<LightComponent>(entity);
    }
    catch(...){
        return ErrorOrVoid("[RenderSystem] Light entity "+ std::to_string(entity) + "missing required components");
    }

    // Unique name for the light
    std::string lightName = "Light_" + std::to_string(entity);
    std::string nodeName = "LightNode_" + std::to_string(entity);
    
    res.ogreLight = sceneManager->createLight(lightName);

    // Get Ogre light type from LightComponent
    Ogre::Light::LightTypes ogreType;
    switch (lightComp->getLightType()) {
        case HUGO::LightType::LT_POINT:
            ogreType = Ogre::Light::LT_POINT;
            break;
        case HUGO::LightType::LT_DIRECTIONAL:
            ogreType = Ogre::Light::LT_DIRECTIONAL;
            break;
        case HUGO::LightType::LT_SPOTLIGHT:
            ogreType = Ogre::Light::LT_SPOTLIGHT;
            break;
        default:
            ogreType = Ogre::Light::LT_POINT;
            break;
    }

    res.ogreLight->setType(ogreType);
    res.ogreLight->setDiffuseColour(Ogre::ColourValue(lightComp->getDiffuse().x, lightComp->getDiffuse().y, lightComp->getDiffuse().z, lightComp->getDiffuse().w));
    res.ogreLight->setSpecularColour(Ogre::ColourValue(lightComp->getSpecular().x, lightComp->getSpecular().y, lightComp->getSpecular().z, lightComp->getSpecular().w));

    // parameters for spot light
    if (lightComp->getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
        res.ogreLight->setSpotlightRange(Ogre::Radian(lightComp->getSpotInner().valueRadians())
            , Ogre::Radian(lightComp->getSpotOuter().valueRadians()), lightComp->getSpotFallOff());

        if (lightComp->getSpotNearClip() > 0) {
            res.ogreLight->setSpotlightNearClipDistance(lightComp->getSpotNearClip());
        }
    }
    
    // parameters for directional light
    if (lightComp->getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
        res.ogreLight->setAttenuation( lightComp->getAttenuation().w, lightComp->getAttenuation().x,  
                                       lightComp->getAttenuation().y, lightComp->getAttenuation().z);
    }

    // shadows
    if(lightComp->getShadowFarDistance() > 0){
        res.ogreLight->setShadowFarDistance(lightComp->getShadowFarDistance());
    }

    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreLight);
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp->getPosition()));

    // for dirrectonal light
    if (lightComp->getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
        res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp->getRotation()));
    }

    lightMap[entity] = res;
    return ErrorOrVoid();
}

#pragma region LightComponentEvents
void LightSystem::onUpdateLightComponent(LightComponent &light)
{
    EntityID entity = light.entity;
    if(!scene->hasComponent<TransformComponent>(entity)){
        std::cerr<<"[LightSystem] Light "<<entity<<" has not trasnform\n";
        return;
    }
    TransformComponent& tr =scene->getComponent<TransformComponent>(entity);
    
    auto it = lightMap.find(entity);
    if (it != lightMap.end()) {
        // update position
        it->second.node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getPosition()));
        
        // for dirrectonal light
        if (light.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
            it->second.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getRotation()));
        }

        // update diffuse and specular
        it->second.ogreLight->setDiffuseColour(Ogre::ColourValue(light.getDiffuse().x, light.getDiffuse().y, light.getDiffuse().z, light.getDiffuse().w));
        it->second.ogreLight->setSpecularColour(Ogre::ColourValue(light.getSpecular().x, light.getSpecular().y, light.getSpecular().z, light.getSpecular().w));
        
        // update attenuation
        it->second.ogreLight->setAttenuation(light.getAttenuation().x ,light.getAttenuation().w, light.getAttenuation().y, light.getAttenuation().z);
        
        if (light.getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
            it->second.ogreLight->setSpotlightRange(Ogre::Radian(light.getSpotInner().valueRadians())
                , Ogre::Radian(light.getSpotOuter().valueRadians()), light.getSpotFallOff());

            if (light.getSpotNearClip() > 0) {
                it->second.ogreLight->setSpotlightNearClipDistance(light.getSpotNearClip());
            }
        }
        
        // parameters for directional light
        if (light.getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
            it->second.ogreLight->setAttenuation( light.getAttenuation().w, light.getAttenuation().x,  
                                        light.getAttenuation().y, light.getAttenuation().z);
        }

        // shadows
        if(light.getShadowFarDistance() > 0){
            it->second.ogreLight->setShadowFarDistance(light.getShadowFarDistance());
        }
    }
}
#pragma endregion