#include <iostream>

#include "CameraMovementSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"


void CameraMovementSystem::init() {
}

void CameraMovementSystem::onLoad() {

}

void CameraMovementSystem::fixedUpdate(double){}

void CameraMovementSystem::update(double dt) {
    for(auto pack : scene->query<CameraComponent, TransformComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();

        auto input = scene->getInput();
        float delta = static_cast<float>(dt);
        if(input->isKeyPressed(SDLK_w)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (-5.f)* delta);
        if(input->isKeyPressed(SDLK_a)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(-5.f)* delta);
        if(input->isKeyPressed(SDLK_s)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (5.f)* delta);
        if(input->isKeyPressed(SDLK_d)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(5.f)* delta);
    
        //std::cout << "CamPos = " << tr.position.x << " " << tr.position.y <<" " << tr.position.z << "\n";
    }

}