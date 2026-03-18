#pragma once

#include <SDL.h>

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace HUGO {
class Input {
  enum KeyState {
    NOT_PRESSED,
    JUST_PRESSED,
    PRESSED,
    JUST_RELEASED
  };
  std::unordered_map<SDL_Keycode, KeyState> keyState;
  std::queue<SDL_KeyCode> toChange;

  std::unordered_map<int, KeyState> mouseState;
  std::queue<int> mouseToChange;

  std::unordered_map<std::string, std::vector<SDL_Keycode>> keysOfAction;

  std::string textInputBuffer;
  bool textInputActive;

  bool quit;

  void processKeyQueue();
  void processMouseQueue();
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
  bool isMouseButtonPressed(int button);

  bool isActionPressed(std::string action);
  bool isQuitPressed();

  bool wasKeyJustPressed(SDL_Keycode key);
  bool wasMouseButtonJustPressed(int button);
  bool wasActionJustPressed(std::string action);

  bool wasKeyJustReleased(SDL_Keycode key);
  bool wasMouseButtonJustReleased(int button);
  bool wasActionJustReleased(std::string action);

  struct MousePosition {
      int x; 
      int y;
  };
  struct MouseRelativePosition {
      float x; 
      float y;
  };
  MousePosition getMousePosition();
  MouseRelativePosition getMouseRelativePosition();
  // Accessor for text input buffer
  const std::string& getTextInput() const;
  // Flush text input buffer once we sent an input
  void clearTextInput();

  // Activate/desactivate Text input support of SDL
  void toggleTextInput(bool state);
};
}; // namespace HUGO
