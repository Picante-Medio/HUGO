#include "EventTestSystem.h"

#include <iostream>

#include "TestComponent.h"

void 
EventTestSystem::init() {

    scene.onAddComponent<TestComponent>().connect(this, &EventTestSystem::testEventA);
    scene.onUpdateComponent<TestComponent>().connect(this, &EventTestSystem::testEventB);
    scene.onRemoveComponent<TestComponent>().connect(this, &EventTestSystem::testEventC);
}

void 
EventTestSystem::onLoad() {

}

void 
EventTestSystem::fixedUpdate(double dt) {

}

void 
EventTestSystem::update(double dt) {

    timer += dt;

    if(timer >= 5.0 ) {

        EntityID ent = scene.createEntity();
        scene.addComponent<TestComponent>(ent);

        for(auto& elem : scene.query<TestComponent>()){ 
            elem.modify<TestComponent>([](TestComponent& testCmp) {
                //testCmp.stringDePrueba = "Prueba modificada";
            });

            //scene.removeComponent<TestComponent>(elem.getConst<TestComponent>().entity);
        }

        timer -= 5.0;
    }
}

void 
EventTestSystem::testEventA(TestComponent& cmp) {
    std::cout << "A: " << cmp.entity << " - " << cmp.stringDePrueba << "\n";
}

void 
EventTestSystem::testEventB(TestComponent& cmp) {
    std::cout << "B: " << cmp.entity << " - " << cmp.stringDePrueba << "\n";
}

void 
EventTestSystem::testEventC(TestComponent& cmp) {
    std::cout << "C : " << cmp.entity << " - " << cmp.stringDePrueba << "\n";
}