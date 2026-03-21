#include "TestSystem.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"

#include <iostream>
#include "TweenComponent.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"
#include "HierarchyTags.h"
void
TestSystem::init() {
    // EntityID ent = scene->createEntity();

    // scene->addAndGetComponent<RenderComponent>(ent).setMeshName("cube.mesh");

    // TransformComponent& trComp = scene->addAndGetComponent<TransformComponent>(ent);
    // TweenComponent& tr= scene->addAndGetComponent<TweenComponent>(ent);

    // // ------------HierarchySystemTest----------
    // e1 = scene->createEntity();
    // TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(e1);
    // transform.position = HUGO::Vector3(0, 0, 0);
    // transform.scale = HUGO::Vector3(0.05f, 0.05f, 0.05f);
    // RenderComponent& rc = scene->addAndGetComponent<RenderComponent>(e1);
    // rc.setMeshName("cube.mesh");
    // rc.setMaterialName("prueba/ColorRosa");
    // scene->addComponent<ParentComponent>(e1);
    // scene->addComponent<TransformChangedTag>(e1);

    // e2 = scene->createEntity();
    // scene->addAndGetComponent<TransformComponent>(e2).position = HUGO::Vector3(-100, 100, 0);
    // RenderComponent& rc2 = scene->addAndGetComponent<RenderComponent>(e2);
    // rc2.setMeshName("cube.mesh");
    // rc2.setMaterialName("prueba/ColorVerde");
    // scene->addAndGetComponent<ParentComponent>(e2).parent=e1;
    // scene->addAndGetComponent<ChildrenComponent>(e1).children.push_back(e2);
    // //----------------------------------------
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
    // TransformComponent& tr=scene->getComponent<TransformComponent>(e1);
    // // tr.position.x += dt * 4;
    // // tr.scale.x+= dt*0.03;
    // tr.rotation *= HUGO::Quaternion(HUGO::Radian(HUGO::Degree(5.0f)), HUGO::Vector3(0,1,0));
    // tr.rotation.normalize();
    // if(!scene->hasComponent<TransformChangedTag>(e1))scene->addComponent<TransformChangedTag>(e1);
    // //----------------------------------------
}

void
TestSystem::fixedUpdate(double) {}