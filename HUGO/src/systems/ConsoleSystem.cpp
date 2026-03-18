#include "ConsoleSystem.h"
#include "PanelComponent.h"
#include <iostream>

void ConsoleSystem::PrintLogs(){
    // Print logs from scene
}

void ConsoleSystem::PrintErrors(){
    // Print errors from scene
}

void ConsoleSystem::update(double dt){

    PrintErrors();
    PrintLogs();

}

void ConsoleSystem::fixedUpdate(double dt){

}

void ConsoleSystem::onLoad(){

}

void ConsoleSystem::init(){

    EntityID e = scene->createEntity();

    scene->addAndGetComponent<PanelComponent>(e);
}