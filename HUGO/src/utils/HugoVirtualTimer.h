#pragma once

#include "SDL.h"

namespace HUGO {
  class VirtualTimer {
  private:
      Uint32 lastTime;
      Uint32 deltaTime;
      bool isPaused;

  public:
      VirtualTimer();
      void update();

      // Stop the timer
      void pause();

      // Resume the timer
      void resume();

      // Reset the timer
      void reset();

      // Getters
      bool getIsPaused() const;
      Uint32 getDeltaTime() const;
  };
}
