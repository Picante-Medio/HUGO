#include <iostream>

#include "CameraMovementSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "HugoInputCodes.h"

EntityID cam1;
EntityID cam2;

bool settingCam1 = false;

void CameraMovementSystem::init() {
    cam1 = scene->createEntity();

    scene->addComponent<TransformComponent>(cam1, [](TransformComponent& comp){
        comp.setPosition(HUGO::Vector3(0,0,30));

    });

    scene->addComponent<CameraComponent>(cam1, [](CameraComponent& comp){
        comp.setMain(true);
    });

    cam2 = scene->createEntity();

    scene->addComponent<TransformComponent>(cam2, [](TransformComponent& comp){
        comp.setPosition(HUGO::Vector3(0,0,200));
    });

    scene->addComponent<CameraComponent>(cam2, [](CameraComponent& comp){
        comp.setMain(false);
    });
}

void CameraMovementSystem::onLoad() {

}

void CameraMovementSystem::fixedUpdate(double){}

void CameraMovementSystem::update(double dt) {
    for(auto pack : scene->query<CameraComponent, TransformComponent>()) {
        if(!pack.getConst<CameraComponent>().isMain()) continue;

        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();

        auto input = scene->getInput();
        float delta = static_cast<float>(dt);
        if(input->isKeyPressed(HUGO::W)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (-0.05f)* delta);
        if(input->isKeyPressed(HUGO::A)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(-0.05f)* delta);
        if(input->isKeyPressed(HUGO::S)) tr.setPosition(tr.getPosition() + tr.getRotation().forward() * (0.05f)* delta);
        if(input->isKeyPressed(HUGO::D)) tr.setPosition(tr.getPosition() + tr.getRotation().right() *(0.05f)* delta);

        //std::cout << "CamPos = " << tr.position.x << " " << tr.position.y <<" " << tr.position.z << "\n";
    }

    if(scene->getInput()->wasKeyJustPressed(HUGO::Q)){
        if(settingCam1){
            scene->getComponent<CameraComponent>(cam1).setMain(true);
            scene->getComponent<CameraComponent>(cam2).setMain(false);
        }
        else{
            scene->getComponent<CameraComponent>(cam1).setMain(false);
            scene->getComponent<CameraComponent>(cam2).setMain(true);
        }
        settingCam1 = !settingCam1;
    }

}