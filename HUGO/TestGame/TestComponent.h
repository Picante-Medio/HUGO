#pragma once

#include "HUGO.h"

HCOMPONENT(TestComponent)
    std::string a;
    int b;

    float c = 0.;

    SERIALIZED_MEMBER(float, testVar)

    DEFAULT_SERIALIZED_MEMBER(std::string, stringDePrueba, "prueba")

END_HCOMPONENT()