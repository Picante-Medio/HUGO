#include "Input.h"
#include <SDL_events.h>
#include <SDL_mouse.h>
#include "HugoTypes.h"
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
                mouseToChange.push(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                mouseState[event.button.button] = JUST_RELEASED;
                mouseToChange.push(event.button.button);
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

    void Input::registerAction(const std::string& actionName,
                               std::vector<HUGO::KeyCode> keys)
    {
        keysOfAction[actionName] = keys;
    }

    void Input::registerAction(const std::string& actionName, HUGO::KeyCode key)
    {
        keysOfAction[actionName] = {key};
    }

    void Input::addKeyToAction(const std::string& actionName, HUGO::KeyCode key)
    {
        keysOfAction[actionName].push_back(key);
    }

    void Input::removeKeyFromAction(const std::string& actionName, HUGO::KeyCode key)
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

    bool Input::isKeyPressed(HUGO::KeyCode key)
    {
        SDL_Keycode SDLKey = hugoKeyToSDLKey(key);
        return keyState[SDLKey] == JUST_PRESSED || keyState[SDLKey] == PRESSED;
    }

    bool Input::isActionPressed(const std::string& action)
    {

        for (HUGO::KeyCode key : keysOfAction[action])
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

    bool Input::wasKeyJustPressed(HUGO::KeyCode key)
    {
        SDL_Keycode SDLKey = hugoKeyToSDLKey(key);
        return keyState[SDLKey] == JUST_PRESSED;
    }

    bool Input::wasActionJustPressed(const std::string& action)
    {
        for (HUGO::KeyCode key : keysOfAction[action])
        {
            if (wasKeyJustPressed(key))
                return true;
        }
        return false;
    }

    bool Input::wasKeyJustReleased(HUGO::KeyCode key)
    {
        SDL_Keycode SDLKey = hugoKeyToSDLKey(key);
        return keyState[SDLKey] == JUST_RELEASED;
    }

    bool Input::wasActionJustReleased(const std::string& action)
    {
        for (HUGO::KeyCode key : keysOfAction[action])
        {
            if (wasKeyJustReleased(key))
                return true;
        }
        return false;
    }

    bool Input::isMouseButtonPressed(HUGO::MouseCode button)
    {
        int SDLButton = hugoMouseInputToSDLMouseInput(button);
        return mouseState[SDLButton] == JUST_PRESSED || mouseState[SDLButton] == PRESSED;
    }

    bool Input::wasMouseButtonJustPressed(HUGO::MouseCode button)
    {
        int SDLButton = hugoMouseInputToSDLMouseInput(button);
        return mouseState[SDLButton] == JUST_PRESSED;
    }

    bool Input::wasMouseButtonJustReleased(HUGO::MouseCode button)
    {
        int SDLButton = hugoMouseInputToSDLMouseInput(button);
        return mouseState[SDLButton] == JUST_RELEASED;
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
        mp.x = static_cast<float>(mpABS.x) / static_cast<float>(w);
        mp.y = static_cast<float>(mpABS.y) / static_cast<float>(h);
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
    int Input::hugoMouseInputToSDLMouseInput(HUGO::MouseCode button)
    {
        int sdlButton;

        switch(button){
        case(MouseCode::MOUSE_LEFT):
            sdlButton = SDL_BUTTON_LEFT;
            break;
        case(MouseCode::MOUSE_MIDDLE):
            sdlButton = SDL_BUTTON_MIDDLE;
            break;
        case(MouseCode::MOUSE_RIGHT):
            sdlButton = SDL_BUTTON_RIGHT;
            break;
        case(MouseCode::MOUSE_BUTTON_X1):
            sdlButton = SDL_BUTTON_X1;
            break;
        case(MouseCode::MOUSE_BUTTON_X2):
            sdlButton = SDL_BUTTON_X2;
            break;
        default:
            sdlButton = 0;
            break;
        }

        return sdlButton;
    }

    SDL_Keycode Input::hugoKeyToSDLKey(HUGO::KeyCode key)
    {
        SDL_KeyCode sdlKey;

        switch(key){
        case(KeyCode::UNKNOWN):
            sdlKey = SDLK_UNKNOWN;
            break;
        case(KeyCode::Q):
            sdlKey = SDLK_q;
            break;
        case(KeyCode::W):
            sdlKey = SDLK_w;
            break;
        case(KeyCode::E):
            sdlKey = SDLK_e;
            break;
        case(KeyCode::R):
            sdlKey = SDLK_r;
            break;
        case(KeyCode::T):
            sdlKey = SDLK_t;
            break;
        case(KeyCode::Y):
            sdlKey = SDLK_y;
            break;
        case(KeyCode::U):
            sdlKey = SDLK_u;
            break;
        case(KeyCode::I):
            sdlKey = SDLK_i;
            break;
        case(KeyCode::O):
            sdlKey = SDLK_o;
            break;
        case(KeyCode::P):
            sdlKey = SDLK_p;
            break;
        case(KeyCode::A):
            sdlKey = SDLK_a;
            break;
        case(KeyCode::S):
            sdlKey = SDLK_s;
            break;
        case(KeyCode::D):
            sdlKey = SDLK_d;
            break;
        case(KeyCode::F):
            sdlKey = SDLK_f;
            break;
        case(KeyCode::G):
            sdlKey = SDLK_g;
            break;
        case(KeyCode::H):
            sdlKey = SDLK_h;
            break;
        case(KeyCode::J):
            sdlKey = SDLK_j;
            break;
        case(KeyCode::K):
            sdlKey = SDLK_k;
            break;
        case(KeyCode::L):
            sdlKey = SDLK_l;
            break;
        case(KeyCode::Z):
            sdlKey = SDLK_z;
            break;
        case(KeyCode::X):
            sdlKey = SDLK_x;
            break;
        case(KeyCode::C):
            sdlKey = SDLK_c;
            break;
        case(KeyCode::V):
            sdlKey = SDLK_v;
            break;
        case(KeyCode::B):
            sdlKey = SDLK_b;
            break;
        case(KeyCode::N):
            sdlKey = SDLK_n;
            break;
        case(KeyCode::M):
            sdlKey = SDLK_m;
            break;
        case(KeyCode::ONE):
            sdlKey = SDLK_1;
            break;
        case(KeyCode::TWO):
            sdlKey = SDLK_2;
            break;
        case(KeyCode::THREE):
            sdlKey = SDLK_3;
            break;
        case(KeyCode::FOUR):
            sdlKey = SDLK_4;
            break;
        case(KeyCode::FIVE):
            sdlKey = SDLK_5;
            break;
        case(KeyCode::SIX):
            sdlKey = SDLK_6;
            break;
        case(KeyCode::SEVEN):
            sdlKey = SDLK_7;
            break;
        case(KeyCode::EIGHT):
            sdlKey = SDLK_8;
            break;
        case(KeyCode::NINE):
            sdlKey = SDLK_9;
            break;
        case(KeyCode::ZERO):
            sdlKey = SDLK_0;
            break;
        case(KeyCode::PANEL_ONE):
            sdlKey = SDLK_KP_1;
            break;
        case(KeyCode::PANEL_TWO):
            sdlKey = SDLK_KP_2;
            break;
        case(KeyCode::PANEL_THREE):
            sdlKey = SDLK_KP_3;
            break;
        case(KeyCode::PANEL_FOUR):
            sdlKey = SDLK_KP_4;
            break;
        case(KeyCode::PANEL_FIVE):
            sdlKey = SDLK_KP_5;
            break;
        case(KeyCode::PANEL_SIX):
            sdlKey = SDLK_KP_6;
            break;
        case(KeyCode::PANEL_SEVEN):
            sdlKey = SDLK_KP_7;
            break;
        case(KeyCode::PANEL_EIGHT):
            sdlKey = SDLK_KP_8;
            break;
        case(KeyCode::PANEL_NINE):
            sdlKey = SDLK_KP_9;
            break;
        case(KeyCode::PANEL_ZERO):
            sdlKey = SDLK_KP_0;
            break;
        case(KeyCode::PANEL_DIVIDE):
            sdlKey = SDLK_KP_DIVIDE;
            break;
        case(KeyCode::PANEL_MULTIPLY):
            sdlKey = SDLK_KP_MULTIPLY;
            break;
        case(KeyCode::PANEL_MINUS):
            sdlKey = SDLK_KP_MINUS;
            break;
        case(KeyCode::PANEL_PLUS):
            sdlKey = SDLK_KP_PLUS;
            break;
        case(KeyCode::PANEL_DECIMAL):
            sdlKey = SDLK_KP_DECIMAL;
            break;
        case(KeyCode::LCTRL):
            sdlKey = SDLK_LCTRL;
            break;
        case(KeyCode::RCTRL):
            sdlKey = SDLK_RCTRL;
            break;
        case(KeyCode::ALT):
            sdlKey = SDLK_LALT;
            break;
        case(KeyCode::ALTGR):
            sdlKey = SDLK_RALT;
            break;
        case(KeyCode::LMAYUS):
            sdlKey = SDLK_LSHIFT;
            break;
        case(KeyCode::RMAYUS):
            sdlKey = SDLK_RSHIFT;
            break;
        case(KeyCode::TAB):
            sdlKey = SDLK_TAB;
            break;
        case(KeyCode::ESC):
            sdlKey = SDLK_ESCAPE;
            break;
        case(KeyCode::SUPR):
            sdlKey = SDLK_DELETE;
            break;
        case(KeyCode::SUPER):
            sdlKey = SDLK_HOME;
            break;
        case(KeyCode::BACKLASH):
            sdlKey = SDLK_BACKSLASH;
            break;
        case(KeyCode::SLASH):
            sdlKey = SDLK_SLASH;
            break;
        case(KeyCode::RETURN):
            sdlKey = SDLK_RETURN;
            break;
        case(KeyCode::BACKSPACE):
            sdlKey = SDLK_BACKSPACE;
            break;
        case(KeyCode::EXCLAMATION):
            sdlKey = SDLK_EXCLAIM;
            break;
        case(KeyCode::DOUBLE_QUOTE):
            sdlKey = SDLK_QUOTEDBL;
            break;
        case(KeyCode::SIMPLE_QUOTE):
            sdlKey = SDLK_QUOTE;
            break;
        case(KeyCode::HASH):
            sdlKey = SDLK_HASH;
            break;
        case(KeyCode::PERCENT):
            sdlKey = SDLK_HASH;
            break;
        case(KeyCode::DOLLAR):
            sdlKey = SDLK_DOLLAR;
            break;
        case(KeyCode::AMPERSAND):
            sdlKey = SDLK_AMPERSAND;
            break;
        case(KeyCode::LEFT_PARENTHESIS):
            sdlKey = SDLK_LEFTPAREN;
            break;
        case(KeyCode::RIGHT_PARENTHESIS):
            sdlKey = SDLK_RIGHTPAREN;
            break;
        case(KeyCode::ASTERISK):
            sdlKey = SDLK_ASTERISK;
            break;
        case(KeyCode::PLUS):
            sdlKey = SDLK_PLUS;
            break;
        case(KeyCode::MINUS):
            sdlKey = SDLK_MINUS;
            break;
        case(KeyCode::COMMA):
            sdlKey = SDLK_COMMA;
            break;
        case(KeyCode::PERIOD):
            sdlKey = SDLK_PERIOD;
            break;
        case(KeyCode::COLON):
            sdlKey = SDLK_COLON;
            break;
        case(KeyCode::SEMICOLON):
            sdlKey = SDLK_SEMICOLON;
            break;
        case(KeyCode::LESS):
            sdlKey = SDLK_LESS;
            break;
        case(KeyCode::EQUALS):
            sdlKey = SDLK_EQUALS;
            break;
        case(KeyCode::GREATER):
            sdlKey = SDLK_GREATER;
            break;
        case(KeyCode::QUESTION):
            sdlKey = SDLK_QUESTION;
            break;
        case(KeyCode::ARROBA):
            sdlKey = SDLK_AT;
            break;
        case(KeyCode::LEFT_BRACKET):
            sdlKey = SDLK_LEFTBRACKET;
            break;
        case(KeyCode::RIGHT_BRACKET):
            sdlKey = SDLK_RIGHTBRACKET;
            break;
        case(KeyCode::CARET):
            sdlKey = SDLK_CARET;
            break;
        case(KeyCode::UNDERSCORE):
            sdlKey = SDLK_UNDERSCORE;
            break;
        case(KeyCode::BACKQUOTE):
            sdlKey = SDLK_BACKQUOTE;
            break;
        case(KeyCode::UP_ARROW):
            sdlKey = SDLK_UP;
            break;
        case(KeyCode::DOWN_ARROW):
            sdlKey = SDLK_DOWN;
            break;
        case(KeyCode::LEFT_ARROW):
            sdlKey = SDLK_LEFT;
            break;
        case(KeyCode::RIGHT_ARROW):
            sdlKey = SDLK_RIGHT;
            break;
        case(KeyCode::F1):
            sdlKey = SDLK_F1;
            break;
        case(KeyCode::F2):
            sdlKey = SDLK_F2;
            break;
        case(KeyCode::F3):
            sdlKey = SDLK_F3;
            break;
        case(KeyCode::F4):
            sdlKey = SDLK_F4;
            break;
        case(KeyCode::F5):
            sdlKey = SDLK_F5;
            break;
        case(KeyCode::F6):
            sdlKey = SDLK_F6;
            break;
        case(KeyCode::F7):
            sdlKey = SDLK_F7;
            break;
        case(KeyCode::F8):
            sdlKey = SDLK_F8;
            break;
        case(KeyCode::F9):
            sdlKey = SDLK_F9;
            break;
        case(KeyCode::F10):
            sdlKey = SDLK_F10;
            break;
        case(KeyCode::F11):
            sdlKey = SDLK_F11;
            break;
        case(KeyCode::F12):
            sdlKey = SDLK_F12;
            break;
        case(KeyCode::SPACE):
            sdlKey = SDLK_SPACE;
            break;
        default:
            sdlKey = SDLK_UNKNOWN;
            break;
        }

        return sdlKey;
    }

}
