#include "Input.h"
#include <SDL_events.h>
#include <SDL_mouse.h>

namespace HUGO {
Input::Input() :  keyState(), toChange(), keysOfAction(), quit(false) 
{}

Input &Input::Instance() {
  static Input instance;
  return instance;
}

void Input::handleInput() {

  SDL_PumpEvents();
  SDL_Event event;

  processKeyQueue();
  processMouseQueue();
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        keyState[event.key.keysym.sym] = JUST_PRESSED;
        break;
      case SDL_KEYUP:
        keyState[event.key.keysym.sym] = JUST_RELEASED;
        break;
      case SDL_MOUSEBUTTONUP:
        mouseState[event.button.button] = JUST_PRESSED;
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouseState[event.button.button] = JUST_RELEASED;
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

bool Input::isKeyPressed(SDL_Keycode key) {
    return keyState[key] == JUST_PRESSED || keyState[key] == PRESSED;
}

bool Input::isActionPressed(std::string action) {
    for(SDL_Keycode key : keysOfAction[action]) {
        if(isKeyPressed(key)) return true;
    }
    return false;
}

bool HUGO::Input::isQuitPressed()
{
    return quit;
}

bool Input::wasKeyJustPressed(SDL_Keycode key) {
    return keyState[key] == JUST_PRESSED;
}

bool Input::wasActionJustPressed(std::string action) {
    for(SDL_Keycode key : keysOfAction[action]) {
        if(wasKeyJustPressed(key)) return true;
    }
    return false;
}

bool Input::wasKeyJustReleased(SDL_Keycode key) {
    return keyState[key] == JUST_RELEASED;
}

bool Input::wasActionJustReleased(std::string action) {
    for(SDL_Keycode key : keysOfAction[action]) {
        if(wasKeyJustReleased(key)) return true;
    }
    return false;
}

bool Input::isMouseButtonPressed(int button) {
    return mouseState[button] == JUST_PRESSED || mouseState[button] == JUST_RELEASED;
}

bool Input::wasMouseButtonJustPressed(int button) {
    return mouseState[button] == JUST_PRESSED;
}

bool Input::wasMouseButtonJustReleased(int button) {
    return mouseState[button] == JUST_RELEASED;
}

Input::MousePosition Input::getMousePosition() {
  MousePosition mp;
  SDL_GetMouseState(&mp.x, &mp.y);
  return mp;
}

void 
Input::processKeyQueue() {
    while(!toChange.empty()) {
        SDL_Keycode key = toChange.front();
        toChange.pop();
        if(keyState[key] == JUST_PRESSED)  keyState[key] = PRESSED;
        else if(keyState[key] == JUST_RELEASED) keyState[key] = NOT_PRESSED;
    }
}


}
