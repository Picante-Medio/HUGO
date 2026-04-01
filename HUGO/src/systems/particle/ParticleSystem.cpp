#include "ParticleSystem.h"
#include "ParticleEmitterComponent.h"
#include "ParticleEmitterComponentTag.h"
#include "TransformComponent.h"

#include <OgreParticleSystemManager.h> 
#include <OgreResourceGroupManager.h>
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

void ParticleSystem::onLoad() {}

void ParticleSystem::createParticleSystem(EntityID id, const HUGO::String& particleName, ParticleEmitterComponent& pe, const HUGO::Vector3& iniPos) {

    //TEMPORAL PARA VER SI SE CARGA EL SISTEMA DE PARTICULAS
    // Ogre::ParticleSystemManager* psManager = Ogre::ParticleSystemManager::getSingletonPtr();
    // if (!psManager) {
    //     std::cerr << "ParticleSystem: ParticleSystemManager is null!" << std::endl;
    //     return;
    // }
    // if (!psManager->getTemplate(particleName)) {
    //     std::cerr << "ParticleSystem: Template '" << particleName << "' not found!" << std::endl;
        
    //     Ogre::ParticleSystemManager::ParticleSystemTemplateIterator it = psManager->getTemplateIterator();
    //     std::cout << "Available particle templates:" << std::endl;
    //     while (it.hasMoreElements()) {
    //         std::cout << "  - " << it.peekNextKey() << std::endl;
    //         it.moveNext();
    //     }
    //     return;
    // }

    HUGO::String n = "ps" + std::to_string(id);

    Ogre::ParticleSystem* ps = sm->createParticleSystem(n, particleName);
    ps->setEmitting(pe.emitting);
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

    node->setPosition({iniPos.x, iniPos.y, iniPos.z});

    ParticleInstance instance;
    instance.ps = ps;
    instance.node = node;
    
    activeParticleSystems[id] = instance;
}

void ParticleSystem::processCreations() {
    for (auto& pack : scene->query<TransformComponent, ParticleEmitterComponent, ParticleEmitterComponentTag>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        ParticleEmitterComponent& pe = pack.getDeprecated<ParticleEmitterComponent&>();
        createParticleSystem(tr.entity, pe.particleName, pe, tr.position);
        scene->removeComponent<ParticleEmitterComponentTag>(tr.entity);
    }
}

void ParticleSystem::destroyParticleSystem(EntityID id) {
    auto it = activeParticleSystems.find(id);
    if (it == activeParticleSystems.end()) return;
    
    ParticleInstance& instance = it->second;
    
    if (instance.node && instance.ps) {
        instance.node->detachAllObjects();
        
        if (sm) {
            sm->destroyParticleSystem(instance.ps);
            sm->destroySceneNode(instance.node);
        }
        
        instance.ps = nullptr;
        instance.node = nullptr;
    }
    
    activeParticleSystems.erase(it);
}

void ParticleSystem::processChanges() {
    
}

void ParticleSystem::fixedUpdate(double dt) {}

void ParticleSystem::update(double dt) {
    //processCreations();
    processChanges();

    // for (auto& [entity, instance] : activeParticleSystems) {
    //     auto& emitter = scene->getComponent<ParticleEmitterComponent>(entity);
    //     if (instance.ps) {
    //         instance.ps->setVisible(emitter.active);
    //         instance.ps->setEmitting(emitter.emitting);
    //     }
    // }
}