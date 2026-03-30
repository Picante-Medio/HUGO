#pragma once

#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "IInput.h"

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
    
    std::unordered_map<std::string, std::vector<SDL_Keycode>> keysOfAction;
    
    std::string textInputBuffer;
    bool textInputActive;
    
    bool quit;
    bool windowChanged;
    
    SDL_Window *sdlWindow = nullptr;
    
    void processKeyQueue();
    void processMouseQueue();
    
    public:
    Input();
    
    // Singleton pattern
    static std::shared_ptr<Input> Instance();
    
    void setSDLWindow(SDL_Window *m_sdlWindow);
    
    // Input handling, needs to be called every update tick
    void handleInput();
    bool isQuitPressed() const;

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
    bool windowHasChanged() const;
    
    bool wasKeyJustPressed(SDL_Keycode key);
    bool wasMouseButtonJustPressed(int button);
    bool wasActionJustPressed(std::string action);
    
    bool wasKeyJustReleased(SDL_Keycode key);
    bool wasMouseButtonJustReleased(int button);
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
};

} // namespace HUGO