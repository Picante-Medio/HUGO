#pragma once

#include "SDL.h"

namespace HUGO {

class VirtualTimer {
private:
    Uint32 lastTime;
    Uint32 deltaTime;
    bool isPaused;

public:
    VirtualTimer() 
        : lastTime(SDL_GetTicks()),
          deltaTime(0), isPaused(false) {}

    
    inline void update() {
        if (!isPaused) {
            Uint32 currentTime = SDL_GetTicks();
            deltaTime = currentTime - lastTime;
            lastTime = currentTime;
        }
        else {
            deltaTime = 0;
        }
    }

    inline double getDeltaTime() const {
        return deltaTime;
    }
   
    // Stop the timer
    void pause() {
        if (!isPaused) {
            isPaused = true;
            lastTime = SDL_GetTicks();
        }
    }

    // Resume the timer
    inline void resume() {
        if (isPaused) {
            isPaused = false;
            lastTime = SDL_GetTicks();
        }
    }

    inline bool getIsPaused() const {
        return isPaused;
    }

    // Reset the timer
    inline void reset() {
        lastTime = SDL_GetTicks();
        deltaTime = 0;
        isPaused = false;
    }
};

}  // namespace HUGO
