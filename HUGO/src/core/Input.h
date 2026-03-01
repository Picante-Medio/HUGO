#pragma once

#include <SDL.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace HUGO {
  class Input {
    std::unordered_map<SDL_KeyCode, bool> keyPressed;
    std::unordered_map<std::string, std::vector<SDL_KeyCode>> keysOfAction;

  public:
    Input();

    //Singleton pattern
    static Input& Instance();

    //Input handling, needs to be called every update tick
    void handleInput();

    bool isKeyPressed(SDL_KeyCode key);
    bool isActionPressed(std::string action);

    bool wasKeyJustPressed(SDL_KeyCode key);
    bool wasActionJustPressed(std::string action);

    bool wasKeyJustReleased(SDL_KeyCode key);
    bool wasActionJustReleased(std::string action);
  };
};
