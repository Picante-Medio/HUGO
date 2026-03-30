#include <iostream>

#include "CameraMovementSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"


void CameraMovementSystem::init() {
}

void CameraMovementSystem::onLoad() {

}

void CameraMovementSystem::fixedUpdate(double dt){
    
}

void CameraMovementSystem::update(double dt) {
    for(auto pack : scene->query<CameraComponent, TransformComponent>()) {
        auto tr = pack.getDeprecated<TransformComponent&>();

        // auto input = HUGO::Input::Instance();

        // if(input.isKeyPressed(SDLK_w)) tr.position = tr.position + tr.rotation.forward() * 5.f * dt;
        // if(input.isKeyPressed(SDLK_a)) tr.position = tr.position + tr.rotation.right() *(-5.f)* dt;
        // if(input.isKeyPressed(SDLK_s)) tr.position = tr.position + tr.rotation.forward() * (-5.f)* dt;
        // if(input.isKeyPressed(SDLK_d)) tr.position = tr.position + tr.rotation.right() *(5.f)* dt;

        
    }

    // std::cout << "Ejecutando CameraMovementSystem\n";
}