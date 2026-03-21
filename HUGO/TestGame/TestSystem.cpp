#include "TestSystem.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"

#include <iostream>
#include "TweenComponent.h"

void
TestSystem::init() {
    EntityID ent = scene->createEntity();

    scene->addAndGetComponent<RenderComponent>(ent).setMeshName("cube.mesh");

    TransformComponent& trComp = scene->addAndGetComponent<TransformComponent>(ent);
    TweenComponent& tr= scene->addAndGetComponent<TweenComponent>(ent);
}
    

void
TestSystem::onLoad() {}

double timer = 0;

bool usedTimer = false;

void
TestSystem::update(double dt) {
    timer += dt;

    if(timer >= 1 && !usedTimer){
        usedTimer = true;

        for(auto& elem : scene->query<TweenComponent, TransformComponent>()){
            TransformComponent& tr = elem.get<TransformComponent&>();
            TweenComponent& tween = elem.get<TweenComponent&>();

            tween.currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::position,
            HUGO::Vector3(0,0,0), HUGO::Vector3(100,0,0), 1.f));

            
            tween.currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(tr, &TransformComponent::rotation, 
            HUGO::Quaternion(HUGO::Radian(HUGO::Degree(0)), HUGO::Vector3(1,0,0)),
            HUGO::Quaternion(HUGO::Radian(HUGO::Degree(-360)), HUGO::Vector3(1,0,0)),
            1.f));
        }
    }
}

void
TestSystem::fixedUpdate(double dt) {}