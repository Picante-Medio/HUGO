#include "TestSystem.h"

#include <iostream>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"
#include "PanelComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "InputFieldComponent.h"
#include "LightComponent.h"
#include "SliderComponent.h"
#include "ParticleEmitterComponent.h"
#include "ParticleEmitterComponentTag.h"

#include "AnimationComponent.h"
#include "AudioComponent.h"
#include "AudioTags.h"

void
TestSystem::init() {
    // EntityID ent = scene->createEntity();

    // scene->addComponent<RenderComponent>(ent, [](RenderComponent& renderComp){
    //     renderComp.setMeshName("cube.mesh");
    // });

    // scene->addComponent<TransformComponent>(ent);
    // scene->addComponent<TweenComponent>(ent);

    // // ------------HierarchySystemTest----------

    e1 = scene->createEntity();
    scene->addComponent<TransformComponent>(e1, [](TransformComponent& t1) {
        t1.setPosition(HUGO::Vector3(0, 0, 0));
        t1.setScale(HUGO::Vector3(0.5f, 0.5f, 0.5f));
    });
    scene->addComponent<RenderComponent>(e1, [](RenderComponent& rc) {
        rc.setMeshName("Sinbad.mesh");
    });
    scene->addComponent<AnimationComponent>(e1, [](AnimationComponent& rc) {
        rc.addAnimation("Dance");
        rc.setEnabled("Dance", true);
        rc.setLoop("Dance", true);
    });

    e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2, [](TransformComponent& t2) {
        t2.setWorldPosition(HUGO::Vector3(2, -4, 0));
    });
    scene->addComponent<RenderComponent>(e2, [](RenderComponent& rc2) {
        rc2.setMeshName("cube.mesh");
        rc2.setMaterialName("prueba/ColorVerde");
    });

    e3 = scene->createEntity(); 
    scene->addComponent<TransformComponent>(e3, [](TransformComponent& t3) {
        t3.setPosition(HUGO::Vector3(-2, 4, 0));
    });
    scene->addComponent<RenderComponent>(e3, [](RenderComponent& rc3) {
        rc3.setMeshName("cube.mesh");
    });

    scene->addComponent<ChildrenComponent>(e1, [this](ChildrenComponent& childrenComp){
        childrenComp.addChild(e2);
        childrenComp.addChild(e3);
    });

    // //----------------------------------------

    // EntityID ui = scene->createEntity();
    // scene->addComponent<TransformComponent>(ui, [](TransformComponent& tr){
    //     //tr.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0));
    //     //tr.rotation.normalize();
    // });
    // scene->addComponent<LightComponent>(ui, [](LightComponent& lg){
    //     //lg.setLightType(HUGO::LightType::LT_SPOTLIGHT);
    //     lg.setLightType(HUGO::LightType::LT_DIRECTIONAL);
    //     lg.setAttenuation(HUGO::Vector4(1.0f, 0.0f, 0.001f, 0.0001f));
    //     lg.setDiffuse(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    //     lg.setSpecular(HUGO::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    // });

    EntityID ui = scene->createEntity();
    scene->addComponent<TransformComponent>(ui, [](TransformComponent& tr){
        tr.setPosition(HUGO::Vector3(0, 100, 100));
        //tr.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0)));
    });
    scene->addComponent<LightComponent>(ui, [](LightComponent& lg){
        lg.setLightType(HUGO::LightType::LT_SPOTLIGHT);
        lg.setDiffuse(HUGO::Vector4(0.8f, 1.8f, 0.8f, 1.0f));
        lg.setSpecular(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    });

    EntityID boton = scene->createEntity();
    scene->addComponent<PanelComponent>(boton, [](PanelComponent& panel){
        panel.overlayName = "testCanvas/SimpleTest";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Background";
        panel.left = 400.0f;
        panel.top = 550.0f;
        panel.width = 64.0f;
        panel.height = 64.0f;
        panel.metricsMode = HUGO::UIMetricsMode::GMM_PIXELS;
    });

    scene->addComponent<ButtonComponent>(boton, [](ButtonComponent& bt){
        bt.onClickCallback = []() {
        std::cout << "Button Clicked!" << std::endl;
        };
        bt.hoverMaterial = "UI/KnobHover";
        bt.pressedMaterial = "UI/KnobPressed";
        bt.normalMaterial = "UI/Knob";
    });

    const HUGO::String& buttonNormalMaterial = scene->getComponent<ButtonComponent>(boton).normalMaterial;

    scene->modifyComponent<PanelComponent>(boton, [buttonNormalMaterial](PanelComponent& panel){
        panel.material = buttonNormalMaterial;
    });

    // EntityID audio = scene->createEntity();
    // scene->addComponent<AudioComponent>(audio, [](AudioComponent& a){
    //     a.isPlaying = true;
    //     a.type = HUGO::AudioTypes::MUSIC;
    //     a.audioName = "lucky";
    //     a.audioId = 0;
    // });
    // scene->addComponent<AudioPlayTag>(audio);

    // EntityID emmiter = scene->createEntity();
    // scene->addComponent<TransformComponent>(emmiter, [](TransformComponent& transform){
    //     transform.position = HUGO::Vector3(0, 0, 0);
    // });
    // scene->addComponent<ParticleEmitterComponent>(emmiter, [](ParticleEmitterComponent& pe){
    //     pe.particleName = "Particle/SmokeBomb";
    //     pe.emitting = true;
    //     pe.active = true;
    // });
    // scene->addComponent<ParticleEmitterComponentTag>(emmiter);
}


void
TestSystem::onLoad() {}

double timer = 0;

bool usedTimer = false;

void
TestSystem::update(double dt) {
    // timer += dt;

    // if(timer >= 1 && !usedTimer){
    //     usedTimer = true;

    //     for(auto& elem : scene->query<TweenComponent, TransformComponent>()){
    //         TransformComponent& tr = elem.getDeprecated<TransformComponent&>();
    //         auto tween = elem.getMutable<TweenComponent>();

    //         tween->currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::position,
    //         HUGO::Vector3(0,0,0), HUGO::Vector3(100,0,0), 1.f));


    //         tween->currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::rotation,
    //         HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(1,0,0)),
    //         HUGO::Quaternion(HUGO::Radian(HUGO::Degree(-360)), HUGO::Vector3(1,0,0)),
    //         1.f));
    //     }
    // }

    // // ------------HierarchySystemTest----------
    // TransformComponent& tr=scene->getComponent<TransformComponent>(e1);
    // tr.setRotation( tr.getRotation()* HUGO::Quaternion(HUGO::Radian(HUGO::Degree(1.0f)), HUGO::Vector3(0,1,0)));

    // TransformComponent& tr2=scene->getComponent<TransformComponent>(e2);
    // tr2.setPosition( tr2.getPosition() + HUGO::Vector3(dt*5, 0, 0 ));
    // //----------------------------------------

    // for(auto& pack : scene->query<LightComponent, TransformComponent>()) {
    //     TransformComponent& tr = pack.get<TransformComponent&>();
    //     LightComponent& lg = pack.get<LightComponent&>();

    //     tr.rotation *= HUGO::Quaternion(HUGO::Radian(HUGO::Degree(5.0f)), HUGO::Vector3(0,1,0));
    //     if(!scene->hasComponent<TransformChangedTag>(tr.entity))scene->addComponent<TransformChangedTag>(tr.entity);
    // }
}

void
TestSystem::fixedUpdate(double) {}
