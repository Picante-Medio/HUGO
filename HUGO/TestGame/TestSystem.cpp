#include "TestSystem.h"

#include <iostream>

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"
#include "HierarchyTags.h"
#include "PanelComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "RenderTags.h"
#include "InputFieldComponent.h"
#include "LightComponent.h"
#include "SliderComponent.h"
#include "UITags.h"


void
TestSystem::init() {
    // EntityID ent = scene->createEntity();

    // scene->addAndGetComponent<RenderComponent>(ent).setMeshName("cube.mesh");

    // TransformComponent& trComp = scene->addAndGetComponent<TransformComponent>(ent);
    // TweenComponent& tr= scene->addAndGetComponent<TweenComponent>(ent);

    // // ------------HierarchySystemTest----------
    e1 = scene->createEntity();
    TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(e1);
    transform.position = HUGO::Vector3(0, 0, 0);
    transform.scale = HUGO::Vector3(0.05f, 0.05f, 0.05f);
    RenderComponent& rc = scene->addAndGetComponent<RenderComponent>(e1);
    rc.setMeshName("cube.mesh");
    rc.setMaterialName("prueba/ColorRosa");
    scene->addComponent<TransformChangedTag>(e1);

    EntityID e2 = scene->createEntity();
    scene->addAndGetComponent<TransformComponent>(e2).position = HUGO::Vector3(-100, 100, 0);
    RenderComponent& rc2 = scene->addAndGetComponent<RenderComponent>(e2);
    rc2.setMeshName("cube.mesh");
    rc2.setMaterialName("prueba/ColorVerde");
    scene->addAndGetComponent<ChildrenComponent>(e1).addChild(e2);
    scene->addComponent<ChildrenCompDirtyTag>(e1);

    // //----------------------------------------
    // EntityID ui = scene->createEntity();
    // TransformComponent& tr = scene->addAndGetComponent<TransformComponent>(ui);
    // //tr.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0));
    // //tr.rotation.normalize();
    // LightComponent& lg = scene->addAndGetComponent<LightComponent>(ui);
    // //lg.setLightType(HUGO::LightType::LT_SPOTLIGHT);
    // lg.setLightType(HUGO::LightType::LT_DIRECTIONAL);
    // lg.setAttenuation(HUGO::Vector4(1.0f, 0.0f, 0.001f, 0.0001f));
    // lg.setDiffuse(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    // lg.setSpecular(HUGO::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
    EntityID ui = scene->createEntity();
    TransformComponent& tr = scene->addAndGetComponent<TransformComponent>(ui);
    tr.position =  HUGO::Vector3(0, 100, 100);
    //tr.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(0,0,0));
    //tr.rotation.normalize();
    LightComponent& lg = scene->addAndGetComponent<LightComponent>(ui);
    lg.setLightType(HUGO::LightType::LT_SPOTLIGHT);
    lg.setDiffuse(HUGO::Vector4(0.8f, 1.8f, 0.8f, 1.0f));
    lg.setSpecular(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
    scene->addComponent<TransformChangedTag>(ui);
    scene->addComponent<LightCreateDirtyTag>(ui);
    scene->addComponent<LightDirtyTag>(ui);

    EntityID boton = scene->createEntity();
    PanelComponent& panel = scene->addAndGetComponent<PanelComponent>(boton);
    panel.overlayName = "testCanvas/SimpleTest";
    panel.containerName = panel.overlayName + "/Container";
    panel.material = "UI/Background";
    panel.left = 0.7f;
    panel.top = 0.5f;

    scene->addComponent<CreatePanelDirtyTag>(boton);
    // pn.overlayName = "testCanvas/ConsoleTextTest";
    // pn.containerName = pn.overlayName + "/Container";
    // pn.material = "UI/Test";
    //ButtonComponent& bt = scene->addAndGetComponent<ButtonComponent>(boton);

    // scene->addComponent<CreatePanelDirtyTag>(boton);
    // //scene->addComponent<CreateButtonDirtyTag>(boton);
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
    //         TransformComponent& tr = elem.get<TransformComponent&>();
    //         TweenComponent& tween = elem.get<TweenComponent&>();

    //         tween.currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::position,
    //         HUGO::Vector3(0,0,0), HUGO::Vector3(100,0,0), 1.f));


    //         tween.currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::rotation,
    //         HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(1,0,0)),
    //         HUGO::Quaternion(HUGO::Radian(HUGO::Degree(-360)), HUGO::Vector3(1,0,0)),
    //         1.f));
    //     }
    // }

      // ------------HierarchySystemTest----------
    TransformComponent& tr=scene->getComponent<TransformComponent>(e1);
    // tr.position.x += dt * 4;
    // tr.scale.x+= dt*0.03;
    tr.rotation *= HUGO::Quaternion(HUGO::Radian(HUGO::Degree(1.0f)), HUGO::Vector3(0,1,0));
    if(!scene->hasComponent<TransformChangedTag>(e1))scene->addComponent<TransformChangedTag>(e1);
    //----------------------------------------


    // for(auto& pack : scene->query<LightComponent, TransformComponent>()) {
    //     TransformComponent& tr = pack.get<TransformComponent&>();
    //     LightComponent& lg = pack.get<LightComponent&>();

    //     tr.rotation *= HUGO::Quaternion(HUGO::Radian(HUGO::Degree(5.0f)), HUGO::Vector3(0,1,0));
    //     if(!scene->hasComponent<TransformChangedTag>(tr.entity))scene->addComponent<TransformChangedTag>(tr.entity);
    // }
}

void
TestSystem::fixedUpdate(double) {}