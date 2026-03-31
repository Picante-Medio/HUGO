#include "Input.h"
#include <SDL_events.h>
#include <SDL_mouse.h>
#include "HugoMath.h"
namespace HUGO
{
    Input::Input() : keyState(), toChange(), keysOfAction(), textInputBuffer(),
                     textInputActive(false), quit(false), windowChanged(false)
    {
    }

    void Input::setSDLWindow(SDL_Window* m_sdlWindow) {
        sdlWindow = m_sdlWindow;
    }

    void Input::handleInput()
    {
        SDL_PumpEvents();
        SDL_Event event;

        processKeyQueue();
        processMouseQueue();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                keyState[event.key.keysym.sym] = JUST_PRESSED;
                toChange.push(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                keyState[event.key.keysym.sym] = JUST_RELEASED;
                toChange.push(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouseState[event.button.button] = JUST_PRESSED;
                toChange.push(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouseState[event.button.button] = JUST_RELEASED;
                toChange.push(event.button.button);
                break;
            case SDL_TEXTINPUT:
                textInputBuffer += event.text.text;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    windowChanged = true;
                }
                break;
            }
        }
    }

    bool HUGO::Input::isQuitPressed() const
    {
        return quit;
    }

    void Input::registerAction(std::string actionName,
                               std::vector<SDL_Keycode> keys)
    {
        keysOfAction[actionName] = keys;
    }

    void Input::registerAction(std::string actionName, SDL_Keycode key)
    {
        keysOfAction[actionName] = {key};
    }

    void Input::addKeyToAction(std::string actionName, SDL_Keycode key)
    {
        keysOfAction[actionName].push_back(key);
    }

    void Input::removeKeyFromAction(std::string actionName, SDL_Keycode key)
    {
        for (auto it = keysOfAction[actionName].begin();
             it != keysOfAction[actionName].end();)
        {
            if (*(it) == key)
                it = keysOfAction[actionName].erase(it);
            else
                ++it;
        }
    }

    bool Input::isKeyPressed(SDL_Keycode key)
    {
        return keyState[key] == JUST_PRESSED || keyState[key] == PRESSED;
    }

    bool Input::isActionPressed(std::string action)
    {
        for (SDL_Keycode key : keysOfAction[action])
        {
            if (isKeyPressed(key))
                return true;
        }
        return false;
    }

    bool Input::windowHasChanged() const
    {
        return windowChanged;
    }

    bool Input::wasKeyJustPressed(SDL_Keycode key)
    {
        return keyState[key] == JUST_PRESSED;
    }

    bool Input::wasActionJustPressed(std::string action)
    {
        for (SDL_Keycode key : keysOfAction[action])
        {
            if (wasKeyJustPressed(key))
                return true;
        }
        return false;
    }

    bool Input::wasKeyJustReleased(SDL_Keycode key)
    {
        return keyState[key] == JUST_RELEASED;
    }

    bool Input::wasActionJustReleased(std::string action)
    {
        for (SDL_Keycode key : keysOfAction[action])
        {
            if (wasKeyJustReleased(key))
                return true;
        }
        return false;
    }

    bool Input::isMouseButtonPressed(int button)
    {
        return mouseState[button] == JUST_PRESSED || mouseState[button] == PRESSED;
    }

    bool Input::wasMouseButtonJustPressed(int button)
    {
        return mouseState[button] == JUST_PRESSED;
    }

    bool Input::wasMouseButtonJustReleased(int button)
    {
        return mouseState[button] == JUST_RELEASED;
    }

    Input::MousePosition Input::getMousePosition()
    {
        MousePosition mp;
        SDL_GetMouseState(&mp.x, &mp.y);
        return mp;
    }
    Input::MouseRelativePosition Input::getMouseRelativePosition()
    {
        MouseRelativePosition mp;
        MousePosition mpABS = getMousePosition();
        int w, h;
        SDL_GetWindowSize(sdlWindow, &w, &h);
        mp.x = static_cast<HUGO::Float>(mpABS.x) / static_cast<HUGO::Float>(w);
        mp.y = static_cast<HUGO::Float>(mpABS.y) / static_cast<HUGO::Float>(h);
        return mp;
    }
    const std::string &Input::getTextInput() const
    {
        return textInputBuffer;
    }

    void Input::clearTextInput()
    {
        textInputBuffer.clear();
    }

    void Input::clearWindowFlag()
    {
        windowChanged = false;
    }

    void Input::toggleTextInput(bool state)
    {
        // avoid repeat calls
        if (textInputActive != state)
        {
            textInputActive = state;
            textInputActive ? SDL_StartTextInput() : SDL_StopTextInput();
        }
    }

    void
    Input::processKeyQueue()
    {
        while (!toChange.empty())
        {
            SDL_Keycode key = toChange.front();
            toChange.pop();
            if (keyState[key] == JUST_PRESSED)
                keyState[key] = PRESSED;
            else if (keyState[key] == JUST_RELEASED)
                keyState[key] = NOT_PRESSED;
        }
    }

    void Input::processMouseQueue()
    {
        while (!mouseToChange.empty())
        {
            int button = mouseToChange.front();
            mouseToChange.pop();
            if (mouseState[button] == JUST_PRESSED)
                mouseState[button] = PRESSED;
            else if (mouseState[button] == JUST_RELEASED)
                mouseState[button] = NOT_PRESSED;
        }
    }


}
