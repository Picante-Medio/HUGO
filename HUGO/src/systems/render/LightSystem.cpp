#include "LightSystem.h"
#include "OgreModule.h"
#include "LightComponent.h"
#include "OgreUtils.h"
#include "TransformComponent.h"
#include "LuaBinder.h"

void LightSystem::init(){
    ogreRoot = Ogre::Root::getSingletonPtr();
    
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    scene.onAddComponent<LightComponent>().connect(this, &LightSystem::registerLightEntity);
    scene.onUpdateComponent<LightComponent>().connect(this, &LightSystem::onUpdateLightComponent);
    scene.onRemoveComponent<LightComponent>().connect(this, &LightSystem::unregisterLightEntity);

    // Basic light
    setAmbientLight(0.6f, 0.6f, 0.6f);
}

void LightSystem::onLoad(){
    LuaBinder::getInstance().registerMethod<LightSystem, void, float, float, float>(*this, &LightSystem::setAmbientLight, "SetAmbientLight");
}

LightSystem::~LightSystem()
{
    //destroy all lights
    for(auto& l: lightMap){
        destroyLight(l.second);
    }
    lightMap.clear();

    sceneManager = nullptr;
}

void LightSystem::update(uint32_t){
    for(auto pack : scene.query<TransformComponent, LightComponent>()){
        TransformComponent transform = pack.getConst<TransformComponent>();
        LightComponent light = pack.getConst<LightComponent>();

        EntityID entID = light.entity;

        //Update lights transform
        lightMap[entID].node->setPosition(HUGO::OgreUtils::vector3ToOgre(transform.getWorldPosition()));
        lightMap[entID].node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transform.getWorldRotation()));
        lightMap[entID].node->setScale(HUGO::OgreUtils::vector3ToOgre(transform.getWorldScale()));
    }
}
// ==================== Register Light Entity ====================
void LightSystem::registerLightEntity(LightComponent& lightComp)
{
    EntityID entity = lightComp.entity;
    LightInfo res;
    
    // Get the transformComponent and LightComponentof the light to set the initial configuration
    if(!scene.hasComponent<TransformComponent>(entity)){
        scene.log(std::make_shared<StringError>("[LightSystem] Light entity "+ std::to_string(entity) + "missing required components"));
        return;
    }

    TransformComponent& transformComp = scene.getComponent<TransformComponent>(entity);

    // Unique name for the light
    std::string lightName = "Light_" + std::to_string(entity);
    std::string nodeName = "LightNode_" + std::to_string(entity);
    
    res.ogreLight = sceneManager->createLight(lightName);

    // Get Ogre light type from LightComponent
    Ogre::Light::LightTypes ogreType;
    switch (lightComp.getLightType()) {
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
    res.ogreLight->setDiffuseColour(Ogre::ColourValue(lightComp.getDiffuse().x, lightComp.getDiffuse().y, lightComp.getDiffuse().z, lightComp.getDiffuse().w));
    res.ogreLight->setSpecularColour(Ogre::ColourValue(lightComp.getSpecular().x, lightComp.getSpecular().y, lightComp.getSpecular().z, lightComp.getSpecular().w));

    // parameters for spot light
    if (lightComp.getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
        res.ogreLight->setSpotlightRange(Ogre::Radian(lightComp.getSpotInner().valueRadians())
            , Ogre::Radian(lightComp.getSpotOuter().valueRadians()), lightComp.getSpotFallOff());

        if (lightComp.getSpotNearClip() > 0) {
            res.ogreLight->setSpotlightNearClipDistance(lightComp.getSpotNearClip());
        }
    }
    
    // parameters for directional light
    if (lightComp.getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
        res.ogreLight->setAttenuation( lightComp.getAttenuation().w, lightComp.getAttenuation().x,  
                                       lightComp.getAttenuation().y, lightComp.getAttenuation().z);
    }

    // shadows
    if(lightComp.getShadowFarDistance() > 0){
        res.ogreLight->setShadowFarDistance(lightComp.getShadowFarDistance());
    }

    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreLight);
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp.getPosition()));

    // for dirrectonal light
    if (lightComp.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
        res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp.getRotation()));
    }

    lightMap[entity] = res;
}

// ==================== Unregister Light Entity ====================
void LightSystem::destroyLight(LightInfo &info)
{
    if(info.node)
    {
        info.node->detachAllObjects();
        sceneManager->destroySceneNode(info.node);
        info.node = nullptr;
    }

    if(info.ogreLight)
    {
        sceneManager->destroyLight(info.ogreLight);
        info.ogreLight = nullptr;
    }
}
void LightSystem::unregisterLightEntity(LightComponent& comp)
{
    EntityID entity = comp.entity;
    destroyLight(lightMap[entity]);
    lightMap.erase(entity);
}

#pragma region LightComponentEvents
void LightSystem::onUpdateLightComponent(LightComponent &light)
{
    EntityID entity = light.entity;
    if(!scene.hasComponent<TransformComponent>(entity)){
        scene.log(std::make_shared<StringError>("[LightSystem] Light " + std::to_string(entity) + " has not trasnform"));
        return;
    }
    TransformComponent& tr =scene.getComponent<TransformComponent>(entity);

    // update position
        lightMap[entity].node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getPosition()));
        
        // for dirrectonal light
        if (light.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
            lightMap[entity].node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getRotation()));
        }

        // update diffuse and specular
        lightMap[entity].ogreLight->setDiffuseColour(Ogre::ColourValue(light.getDiffuse().x, light.getDiffuse().y, light.getDiffuse().z, light.getDiffuse().w));
        lightMap[entity].ogreLight->setSpecularColour(Ogre::ColourValue(light.getSpecular().x, light.getSpecular().y, light.getSpecular().z, light.getSpecular().w));
        
        if (light.getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
            lightMap[entity].ogreLight->setSpotlightRange(Ogre::Radian(light.getSpotInner().valueRadians())
                , Ogre::Radian(light.getSpotOuter().valueRadians()), light.getSpotFallOff());

            if (light.getSpotNearClip() > 0) {
                lightMap[entity].ogreLight->setSpotlightNearClipDistance(light.getSpotNearClip());
            }
        }
        
        // parameters for directional light
        if (light.getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
            lightMap[entity].ogreLight->setAttenuation( light.getAttenuation().w, light.getAttenuation().x,  
                                        light.getAttenuation().y, light.getAttenuation().z);
        }

        // shadows
        if(light.getShadowFarDistance() > 0){
            lightMap[entity].ogreLight->setShadowFarDistance(light.getShadowFarDistance());
        }
}
#pragma endregion

void LightSystem::setAmbientLight(HUGO::Float r, HUGO::Float g, HUGO::Float b)
{
    sceneManager->setAmbientLight(Ogre::ColourValue(r, g, b));
}