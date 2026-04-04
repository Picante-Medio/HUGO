#pragma once

#include <string>
#include <vector>

#include <SDL.h>

namespace HUGO
{
    class IInput {

    public:
        // Action adition and elimination
        virtual void registerAction(std::string actionName, std::vector<SDL_Keycode> keys) = 0;
        virtual void registerAction(std::string actionName, SDL_Keycode key) = 0;
        virtual void addKeyToAction(std::string actionName, SDL_Keycode key) = 0;

        //!!Linear in the number of actions
        virtual void removeKeyFromAction(std::string actionName, SDL_Keycode key) = 0;

        // Accessors for input state
        virtual bool isKeyPressed(SDL_Keycode key) = 0;
        virtual bool isMouseButtonPressed(int button) = 0;

        virtual bool isActionPressed(std::string action) = 0;
        virtual bool windowHasChanged() const = 0;

        virtual bool wasKeyJustPressed(SDL_Keycode key) = 0;
        virtual bool wasMouseButtonJustPressed(int button) = 0;
        virtual bool wasActionJustPressed(std::string action) = 0;

        virtual bool wasKeyJustReleased(SDL_Keycode key) = 0;
        virtual bool wasMouseButtonJustReleased(int button) = 0;
        virtual bool wasActionJustReleased(std::string action) = 0;

        struct MousePosition
        {
            int x;
            int y;
        };
        struct MouseRelativePosition
        {
            float x;
            float y;
        };

        virtual MousePosition getMousePosition() = 0;
        virtual MouseRelativePosition getMouseRelativePosition() = 0;

        // Accessor for text input buffer
        virtual const std::string &getTextInput() const = 0;

        virtual void clearTextInput() = 0;
        virtual void clearWindowFlag() = 0;
        
        // Activate/desactivate Text input support of SDL
        virtual void toggleTextInput(bool state) = 0;
        
    };

} // namespace HUGO