#include <iostream>

#include "CameraMovementSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "RenderTags.h"


void CameraMovementSystem::init() {
}

void CameraMovementSystem::onLoad() {

}

void CameraMovementSystem::fixedUpdate(double dt){
    
}



void CameraMovementSystem::update(double dt) {
    for(auto pack : scene->query<CameraComponent, TransformComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();

         auto input = scene->getInput();

         if(input->isKeyPressed(SDLK_w)) tr.position = tr.position + tr.rotation.forward() * (-5.f)* dt;
         if(input->isKeyPressed(SDLK_a)) tr.position = tr.position + tr.rotation.right() *(-5.f)* dt;
         if(input->isKeyPressed(SDLK_s)) tr.position = tr.position + tr.rotation.forward() * (5.f)* dt;
         if(input->isKeyPressed(SDLK_d)) tr.position = tr.position + tr.rotation.right() *(5.f)* dt;

         scene->addComponent<CameraDirtyTag>(tr.entity);
        
         std::cout << "CamPos = " << tr.position.x << " " << tr.position.y <<" " << tr.position.z << "\n";
    }

}