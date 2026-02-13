#include "Input.h"
#include <SDL_events.h>

namespace HUGO {
Input::Input() : keysOfAction(), keyPressed(), previousKeyPressed(), quit(false) {}

Input &Input::Instance() {
  static Input instance;
  return instance;
}

void Input::handleInput() {
  previousKeyPressed = keyPressed;

  SDL_PumpEvents();
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
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

void Input::registerAction(std::string actionName,
                           std::vector<SDL_Keycode> keys) {
  keysOfAction[actionName] = keys;
}

void Input::registerAction(std::string actionName, SDL_Keycode key) {
  keysOfAction[actionName] = {key};
}

void Input::addKeyToAction(std::string actionName, SDL_Keycode key) {
  keysOfAction[actionName].push_back(key);
}

void Input::removeKeyFromAction(std::string actionName, SDL_Keycode key) {
  for (auto it = keysOfAction[actionName].begin();
       it != keysOfAction[actionName].end();) {
    if (*(it) == key)
      it = keysOfAction[actionName].erase(it);
    else
      ++it;
  }
}

bool Input::isKeyPressed(SDL_Keycode key) { return keyPressed[key]; }

bool Input::isActionPressed(std::string action) {
  for (SDL_Keycode key : keysOfAction[action])
    if (keyPressed[key])
      return true;

  return false;
}

bool HUGO::Input::isQuitPressed()
{
    return quit;
}

bool Input::wasKeyJustPressed(SDL_Keycode key) {
  return previousKeyPressed[key];
}

bool Input::wasActionJustPressed(std::string action) {
  for (SDL_Keycode key : keysOfAction[action])
    if (previousKeyPressed[key])
      return true;

  return false;
}

bool Input::wasKeyJustReleased(SDL_Keycode key) {
  return previousKeyPressed[key] && !keyPressed[key];
}

bool Input::wasActionJustReleased(std::string action) {
  for (SDL_Keycode key : keysOfAction[action])
    if (wasKeyJustReleased(key))
      return true;

  return false;
}

} // namespace HUGO
