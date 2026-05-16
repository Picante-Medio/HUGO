#include "TestSystem.h"

#include <iostream>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"
#include "ParentComponent.h"
#include "PanelComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"
#include "LightComponent.h"
#include "SliderComponent.h"
#include "ParticleEmitterComponent.h"

#include "AnimationComponent.h"
#include "AudioComponent.h"

#include "ChangeSceneMessage.h"
void
TestSystem::init() {
    // EntityID ent = scene.createEntity();

    // scene.addComponent<RenderComponent>(ent, [](RenderComponent& renderComp){
    //     renderComp.setMeshName("cube.mesh");
    // });

    // scene.addComponent<TransformComponent>(ent);
    // scene.addComponent<TweenComponent>(ent);

    // // ------------HierarchySystemTest----------

    e1 = scene.createEntity();
    scene.addComponent<TransformComponent>(e1, [](TransformComponent& t1) {
        t1.position = {0, 0, 0};
        t1.scale = {0.5f, 0.5f, 0.5f};
    });
    scene.addComponent<RenderComponent>(e1, [](RenderComponent& rc) {
        rc.setMeshName("Sinbad.mesh");
    });
    scene.addComponent<AnimationComponent>(e1, [](AnimationComponent& rc) {
        rc.addAnimation("Dance", true, true);
    });

    e2 = scene.createEntity();
    scene.addComponent<TransformComponent>(e2, [](TransformComponent& t2) {
        t2.position = {2, -4, 0};
    });
    scene.addComponent<RenderComponent>(e2, [](RenderComponent& rc2) {
        rc2.setMeshName("cube.mesh");
        rc2.setMaterialName("prueba/ColorVerde");
    });

    e3 = scene.createEntity(); 
    scene.addComponent<TransformComponent>(e3, [](TransformComponent& t3) {
        t3.position = {-2, 4, 0};
    });
    scene.addComponent<RenderComponent>(e3, [](RenderComponent& rc3) {
        rc3.setMeshName("cube.mesh");
    });

    scene.addComponent<ParentComponent>(e2, [this](ParentComponent& parentComp){
        parentComp.parent = e1;
    });

    scene.addComponent<ParentComponent>(e3, [this](ParentComponent& parentComp){
        parentComp.parent = e1;
    });

    // //----------------------------------------

    // EntityID ui = scene.createEntity();
    // scene.addComponent<TransformComponent>(ui, [](TransformComponent& tr){
    //     //tr.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0));
    //     //tr.rotation.normalize();
    // });
    EntityID light = scene.createEntity();
    
    scene.addComponent<TransformComponent>(light);

    scene.addComponent<LightComponent>(light, [](LightComponent& lg){
         lg.setLightType(HUGO::LightType::LT_POINT);
        lg.setAttenuation(HUGO::Vector4(1.0f, 0.0f, 0.001f, 0.0001f));
         lg.setDiffuse(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
         lg.setSpecular(HUGO::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
     });

    EntityID ui = scene.createEntity();
    scene.addComponent<TransformComponent>(ui, [](TransformComponent& tr){
        tr.position = {0, 100, 100};
        //tr.rotation = (HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0)));
    });
    scene.addComponent<LightComponent>(ui, [](LightComponent& lg){
        lg.setLightType(HUGO::LightType::LT_SPOTLIGHT);
        lg.setDiffuse(HUGO::Vector4(0.8f, 1.8f, 0.8f, 1.0f));
        lg.setSpecular(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    });

    // EntityID audio = scene.createEntity();
    // scene.addComponent<AudioComponent>(audio, [](AudioComponent& a){
    //     a.isPlaying = true;
    //     a.type = HUGO::AudioTypes::MUSIC;
    //     a.audioName = "lucky";
    //     a.audioId = 0;
    // });
    // scene.addComponent<AudioPlayTag>(audio);

    // EntityID emmiter = scene.createEntity();
    // scene.addComponent<TransformComponent>(emmiter, [](TransformComponent& transform){
    //     transform.setPosition(HUGO::Vector3(10.f, 0.0f, 0.0f));
    // });
    // scene.addComponent<ParticleEmitterComponent>(emmiter, [](ParticleEmitterComponent& pe){
    //     pe.particleName = "Particle/Smoke";
    //     pe.emitting = true;
    //     pe.active = true;
    //     pe.material = "debug/Smoke";
    // });
    
    auto tweenEnt = scene.createEntity();
    scene.addComponent<TransformComponent>(tweenEnt);

    scene.addComponent<TweenComponent>(tweenEnt, [](TweenComponent&){
        
    });
}


void
TestSystem::onLoad() {}

double timer = 0;

bool usedTimer = false;

void
TestSystem::update(uint32_t dt) {

    for(auto& elem: scene.query<TransformComponent, LightComponent>()){
        elem.modify<TransformComponent>([dt](TransformComponent& tr){
            tr.position.z += dt;
        });
    }
}

void
TestSystem::fixedUpdate(uint32_t) {}
