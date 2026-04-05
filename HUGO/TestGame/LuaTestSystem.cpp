#include "LuaTestSystem.h"
#include "LuaBinder.h"

void
LuaTestSystem::foo() {
    std::cout << "Estoy imprimiendo desde lua como función de c y encima en el juego buah ya ves chaval\n";
}

void 
LuaTestSystem::onLoad() {
    LuaBinder::getInstance()->registerMethod<LuaTestSystem, void>(*this, &LuaTestSystem::foo, "foo");
}

void 
LuaTestSystem::update(double dt) {
    currentTime += dt;

    if(currentTime >= nextTime) {
        LuaBinder::getInstance()->interpretString("foo()");
        currentTime = 0.;
    }
}
