#pragma once

#include <SDL.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace HUGO {
class Input {
  std::unordered_map<SDL_Keycode, bool> keyPressed;
  std::unordered_map<SDL_Keycode, bool> previousKeyPressed;
  std::unordered_map<std::string, std::vector<SDL_Keycode>> keysOfAction;
  bool quit;
public:
  Input();

  // Singleton pattern
  static Input &Instance();

  // Input handling, needs to be called every update tick
  void handleInput();

  // Action adition and elimination
  void registerAction(std::string actionName, std::vector<SDL_Keycode> keys);
  void registerAction(std::string actionName, SDL_Keycode key);
  void addKeyToAction(std::string actionName, SDL_Keycode key);

  //!!Linear in the number of actions
  void removeKeyFromAction(std::string actionName, SDL_Keycode key);

  // Accessors for input state
  bool isKeyPressed(SDL_Keycode key);
  bool isActionPressed(std::string action);
  bool isQuitPressed();

  bool wasKeyJustPressed(SDL_Keycode key);
  bool wasActionJustPressed(std::string action);

  bool wasKeyJustReleased(SDL_Keycode key);
  bool wasActionJustReleased(std::string action);

  struct MousePosition {
      int x; 
      int y;
  };

  MousePosition getMousePosition();
};
}; // namespace HUGO
