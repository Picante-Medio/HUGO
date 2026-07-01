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

    class ErrorOrVoid;

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
    
    std::unordered_map<std::string, std::vector<HUGO::KeyCode>> keysOfAction;
    
    std::string textInputBuffer;
    bool textInputActive;
    
    bool quit;
    bool windowChanged;

    int relX = 0;
    int relY = 0;
    
    SDL_Window *sdlWindow = nullptr;
    
    void processKeyQueue();
    void processMouseQueue();

    // Translators from hugo codes to SDL codes
    int hugoMouseInputToSDLMouseInput(HUGO::MouseCode button);
    SDL_Keycode hugoKeyToSDLKey(HUGO::KeyCode key);
    
    public:
    Input();
    
    void setSDLWindow(SDL_Window *m_sdlWindow);
    
    // Input handling, needs to be called every update tick
    void handleInput();
    bool isQuitPressed() const;

    // Action adition and removal
    void registerAction(const std::string& actionName, std::vector<HUGO::KeyCode> keys);
    void registerAction(const std::string& actionName, HUGO::KeyCode key);
    void addKeyToAction(const std::string& actionName, HUGO::KeyCode key);
    
    //!!Linear in the number of actions
    void removeKeyFromAction(const std::string& actionName, HUGO::KeyCode key);
    
    // Accessors for input state
    bool isKeyPressed(HUGO::KeyCode key);
    bool isMouseButtonPressed(HUGO::MouseCode button);
    
    bool isActionPressed(const std::string& action);
    bool windowHasChanged() const;
    
    bool wasKeyJustPressed(HUGO::KeyCode key);
    bool wasMouseButtonJustPressed(HUGO::MouseCode button);
    bool wasActionJustPressed(const std::string& action);
    
    bool wasKeyJustReleased(HUGO::KeyCode key);
    bool wasMouseButtonJustReleased(HUGO::MouseCode button);
    bool wasActionJustReleased(const std::string& action);
    
    MousePosition getMousePosition();
    MouseRelativePosition getMouseRelativePosition();
    ErrorOrVoid captureMouse(bool capture);
    MousePosition getMouseDelta();
    
    // Accessor for text input buffer
    const std::string &getTextInput() const;
    // Flush text input buffer once we sent an input
    void clearTextInput();
    
    void clearWindowFlag();
    
    // Activate/desactivate Text input support of SDL
    void toggleTextInput(bool state);
};

} // namespace HUGO
