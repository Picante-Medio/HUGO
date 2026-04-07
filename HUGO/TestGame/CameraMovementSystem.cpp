#include <iostream>

#include "CameraMovementSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "HugoInputCodes.h"

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
        if(input->isKeyPressed(HUGO::W)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (-0.05f)* delta);
        if(input->isKeyPressed(HUGO::A)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(-0.05f)* delta);
        if(input->isKeyPressed(HUGO::S)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (0.05f)* delta);
        if(input->isKeyPressed(HUGO::D)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(0.05f)* delta);

        //std::cout << "CamPos = " << tr.position.x << " " << tr.position.y <<" " << tr.position.z << "\n";
    }

}