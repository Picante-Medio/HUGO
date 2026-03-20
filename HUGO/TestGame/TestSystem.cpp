#include "TestSystem.h"
#include "AudioComponent.h"
#include "RenderComponent.h"
#include "TweenComponent.h"

#include <iostream>
#include "TweenComponent.h"

void
TestSystem::init() {
    EntityID ent = scene->createEntity();

    auto trComp = scene->addAndGetComponent<AudioComponent>(ent);
    scene->addAndGetComponent<TweenComponent>(ent).currentTweens.push_back(HUGO::TweenFactory<EaseInBackTween>::makeTween(trComp, &AudioComponent::volume, 100, 0, 5));
}   

void
TestSystem::onLoad() {}

void
TestSystem::update(double dt) {
    for(auto& pack : scene->query<AudioComponent,TweenComponent>()) {
        std::cout << pack.get<AudioComponent&>().volume << '\n';
    }
}

void
TestSystem::fixedUpdate(double dt) {}