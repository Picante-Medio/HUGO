#pragma once

#include <string>
#include <vector>

#include "HugoInputCodes.h"

namespace HUGO
{
    class IInput {

    public:
        // Action adition and elimination
        virtual void registerAction(const std::string& actionName, std::vector<HUGO::KeyCode> keys) = 0;
        virtual void registerAction(const std::string& actionName, HUGO::KeyCode key) = 0;
        virtual void addKeyToAction(const std::string& actionName, HUGO::KeyCode key) = 0;

        //!!Linear in the number of actions
        virtual void removeKeyFromAction(const std::string& actionName, HUGO::KeyCode key) = 0;

        // Accessors for input state
        virtual bool isKeyPressed(HUGO::KeyCode key) = 0;
        virtual bool isMouseButtonPressed(HUGO::MouseCode button) = 0;

        virtual bool isActionPressed(const std::string& action) = 0;
        virtual bool windowHasChanged() const = 0;

        virtual bool wasKeyJustPressed(HUGO::KeyCode key) = 0;
        virtual bool wasMouseButtonJustPressed(HUGO::MouseCode button) = 0;
        virtual bool wasActionJustPressed(const std::string& action) = 0;

        virtual bool wasKeyJustReleased(HUGO::KeyCode key) = 0;
        virtual bool wasMouseButtonJustReleased(HUGO::MouseCode button) = 0;
        virtual bool wasActionJustReleased(const std::string& action) = 0;

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