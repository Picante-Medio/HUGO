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
      inline void resume();

      // Reset the timer
      inline void reset();

      // Getters
      inline bool getIsPaused() const;
      inline Uint32 getDeltaTime() const;
  };
}
