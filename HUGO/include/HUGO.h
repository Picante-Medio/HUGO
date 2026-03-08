#pragma once
/*
    Este header está pensado para se incluido como desarrollador, tiene todo lo que se necesita
    para desarrollar componentes y sistemas
*/

#include <string>
#include <functional>
#include <unordered_map>
#include <any>

#include "IScene.h"
#include "Component.h"
#include "ModuleComponentFactory.h"
#include "HugoComponentMacros.h"

// Declaración de prueba para ver si los componentes reflejan

// HCOMPONENT(MiComponent)
//     std::string a;
//     int b;

//     float c = 0.;

//     SERIALIZED_MEMBER(float, testVar)

//     SERIALIZED_MEMBER(std::string, stringDePrueba)

// END_HCOMPONENT()