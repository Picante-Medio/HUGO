#include "ParticleSystem.h"
#include "ParticleEmitterComponent.h"
#include "TransformComponent.h"

#include "OgreParticleSystem.h"
#include "OgreParticleEmitter.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreRoot.h"

ParticleSystem::~ParticleSystem() {
    for (auto& [entity, instance] : activeParticleSystems) {
        if (instance.node && instance.ps) {
            // deatch
            instance.node->detachAllObjects();

            // destroy ogre particle system
            sm->destroyParticleSystem(instance.ps);

            // destroy ogre scenenode
            sm->getRootSceneNode()->removeChild(instance.node);
            sm->destroySceneNode(instance.node);
        }
    }
    activeParticleSystems.clear();
}
void ParticleSystem::init() {
    sm = Ogre::Root::getSingletonPtr()->_getCurrentSceneManager();
}

void ParticleSystem::onLoad() {
    // Aquí podrías inicializar plantillas de partículas si quieres
}

void ParticleSystem::createParticleSystem(EntityID id, const HUGO::String& particleName, 
    ParticleEmitterComponent& pe, const HUGO::Vector3& iniPos) {
    HUGO::String n = "ps" + std::to_string(id);

    Ogre::ParticleSystem* ps = sm->createParticleSystem(n, particleName);
    Ogre::SceneNode* node = sm->getRootSceneNode()->createChildSceneNode();

    node->attachObject(ps);

    //get the emitter
    Ogre::ParticleEmitter* e =  ps->getEmitter(0);

    // read the datas
    HUGO::Float emissionRate = e->getEmissionRate();
    Ogre::Vector3 direction = e->getDirection();
    Ogre::ColourValue colour = e->getColour();
    HUGO::Float timeToLive = e->getTimeToLive();

    // store
    pe.emissionRate = emissionRate;
    pe.direction = HUGO::Vector3(direction.x, direction.y, direction.z);
    pe.colour = HUGO::Vector4(colour.r, colour.g, colour.b, colour.a);
    pe.timeToLive = timeToLive;

    e->setPosition({iniPos.x, iniPos.y, iniPos.z});
}

void ParticleSystem::processCreations() {
    for (auto& pack : scene->query<TransformComponent, ParticleEmitterComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        ParticleEmitterComponent& pe = pack.getDeprecated<ParticleEmitterComponent&>();
        createParticleSystem(tr.entity, pe.particleName, 
        pe, tr.position);
    }
}

void ParticleSystem::processChanges() {
    
}

void ParticleSystem::fixedUpdate(double dt) {
    // Opcional: si quieres lógica con pasos fijos
}

void ParticleSystem::update(double dt) {
    for (auto& [entity, instance] : activeParticleSystems) {
        auto& emitter = scene->getComponent<ParticleEmitterComponent>(entity);
        if (instance.ps) {
            instance.ps->setVisible(emitter.active);
        }
    }
}