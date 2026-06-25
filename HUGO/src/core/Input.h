#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <SDL.h>

#include "IInput.h"
#include "HugoInputCodes.h"

namespace HUGO {

    class Input : public HUGO::IInput
    {
        enum KeyState
        {
            NOT_PRESSED,
            JUST_PRESSED,
            PRESSED,
            JUST_RELEASED
    };
    std::unordered_map<SDL_Keycode, KeyState> keyState;
    std::queue<SDL_Keycode> toChange;
    
    std::unordered_map<int, KeyState> mouseState;
    std::queue<int> mouseToChange;
    
    // std::unordered_map<SDL_Keycode, KeyState> consoleKeyState;
    // std::queue<SDL_Keycode> consoletoChange;

    std::unordered_map<std::string, std::vector<HUGO::KeyCode>> keysOfAction;
    
    std::string textInputBuffer;
    bool textInputActive;
    
    bool quit;
    bool windowChanged;
    // bool consoleActive;
    
    SDL_Window *sdlWindow = nullptr;
    
    void processKeyQueue();
    void processMouseQueue();
    // void processConsoleQueue();

    // Translators from hugo codes to SDL codes
    int hugoMouseInputToSDLMouseInput(HUGO::MouseCode button);
    SDL_Keycode hugoKeyToSDLKey(HUGO::KeyCode key);
    
    public:
    Input();
    
    void setSDLWindow(SDL_Window *m_sdlWindow);
    
    // Input handling, needs to be called every update tick
    void handleInput();
    bool isQuitPressed() const;

    // Action adition and elimination
    void registerAction(std::string actionName, std::vector<HUGO::KeyCode> keys);
    void registerAction(std::string actionName, HUGO::KeyCode key);
    void addKeyToAction(std::string actionName, HUGO::KeyCode key);
    
    //!!Linear in the number of actions
    void removeKeyFromAction(std::string actionName, HUGO::KeyCode key);
    
    // Accessors for input state
    bool isKeyPressed(HUGO::KeyCode key);
    bool isMouseButtonPressed(HUGO::MouseCode button);
    
    bool isActionPressed(std::string action);
    bool windowHasChanged() const;
    
    bool wasKeyJustPressed(HUGO::KeyCode key);
    bool wasMouseButtonJustPressed(HUGO::MouseCode button);
    bool wasActionJustPressed(std::string action);
    
    bool wasKeyJustReleased(HUGO::KeyCode key);
    bool wasMouseButtonJustReleased(HUGO::MouseCode button);
    bool wasActionJustReleased(std::string action);
    
    MousePosition getMousePosition();
    MouseRelativePosition getMouseRelativePosition();
    
    
    // Accessor for text input buffer
    const std::string &getTextInput() const;
    // Flush text input buffer once we sent an input
    void clearTextInput();
    
    void clearWindowFlag();
    
    // Activate/desactivate Text input support of SDL
    void toggleTextInput(bool state);

    //Activate/desactivate consoleInput
    // void setConsoleActive(bool active);
    // bool isConsoleKeyPressed(HUGO::KeyCode key);
    // bool wasConsoleKeyJustPressed(HUGO::KeyCode key);

};

} // namespace HUGO
