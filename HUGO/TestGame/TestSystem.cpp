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

    TransformComponent trComp = scene->addAndGetComponent<TransformComponent>(ent);
    scene->addAndGetComponent<TweenComponent>(ent).currentTweens.push_back(HUGO::TweenFactory<EaseOutElasticTween>::makeTween(trComp, &TransformComponent::position,
         HUGO::Vector3(0,0,0), HUGO::Vector3(100,0,0), 1.f));
}   

void
TestSystem::onLoad() {}

void
TestSystem::update(double dt) {

}

void
TestSystem::fixedUpdate(double dt) {}