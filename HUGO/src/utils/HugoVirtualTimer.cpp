#include "HugoVirtualTimer.h"

namespace HUGO {

  VirtualTimer::VirtualTimer()
      : lastTime(SDL_GetTicks()), deltaTime(0), isPaused(false) 
  {}

  void 
  VirtualTimer::update() {
    if (!isPaused) {
      Uint32 currentTime = SDL_GetTicks();
      deltaTime = currentTime - lastTime;
      lastTime = currentTime;
    } else {
      deltaTime = 0;
    }
  }

  Uint32 
  VirtualTimer::getDeltaTime() const { return deltaTime; }

  void VirtualTimer::pause() {
    if (!isPaused) {
      isPaused = true;
      lastTime = SDL_GetTicks();
    }
  }

  void 
  VirtualTimer::resume() {
    if (isPaused) {
      isPaused = false;
      lastTime = SDL_GetTicks();
    }
  }

  bool VirtualTimer::getIsPaused() const { return isPaused; }

  void 
  VirtualTimer::reset() {
    lastTime = SDL_GetTicks();
    deltaTime = 0;
    isPaused = false;
  }

}
