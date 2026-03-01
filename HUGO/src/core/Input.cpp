#include "Input.h"
#include <SDL_events.h>

namespace HUGO {
Input::Input() : keysOfAction(), keyPressed() {}

Input &Input::Instance() {
  static Input instance;
  return instance;
}

void Input::handleInput() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
  
    switch (event.type) {
    case SDL_KEYDOWN:
      keyPressed[event.key.keysym.sym] = true;
      break;

    case SDL_KEYUP:
      keyPressed[event.key.keysym.sym] = false;
      break;

    default:
      break;
    }
  }
}

bool 
Input::isKeyPressed(SDL_Keycode key) {
  return keyPressed[key];
}

bool 
Input::isActionPressed(std::string action) {
  for(SDL_Keycode key : keysOfAction[action])
    if(keyPressed[key]) return true;

  return false;
}

bool
Input::wasKeyJustPressed(SDL_Keycode key) {

}

bool 
Input::wasActionJustPressed(std::string action) {}

bool 
Input::wasKeyJustReleased(SDL_Keycode key) {}

bool 
Input::wasActionJustReleased(std::string action) {}

} // namespace HUGO
