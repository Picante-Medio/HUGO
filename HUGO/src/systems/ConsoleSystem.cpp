#include "ConsoleSystem.h"
#include "PanelComponent.h"
#include <iostream>

void ConsoleSystem::PrintLogs(){
    ErrorOrType log = scene->getNextLog();

    while (!log.isError())
    {
        // Imprimir
        log = scene->getNextLog();
    }
    
}

void ConsoleSystem::PrintErrors(){
    ErrorOrType error = scene->getNextError();

    while (!error.isError())
    {
        // Imprimir
        error = scene->getNextError();
    }
}

void ConsoleSystem::update(double dt){

    // Letras color rojo
    PrintErrors();

    // Letras color blanco
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