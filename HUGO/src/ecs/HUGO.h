#pragma once
/*
    Este header está pensado para se incluido como desarrollador, tiene todo lo que se necesita
    para desarrollar componentes y sistemas
*/

#include <string>

#include "Component.h"

#define COMPONENT(COMPONENT_NAME) \
class COMPONENT_NAME: Component { \
public: \
    static const std::string COMPONENT_NAME = #COMPONENT_NAME; \

// por añadir: struct estático autoregistrador
// 
