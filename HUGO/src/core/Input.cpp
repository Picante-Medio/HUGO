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
                               std::vector<HUGO::KeyCode> keys)
    {
        keysOfAction[actionName] = keys;
    }

    void Input::registerAction(std::string actionName, HUGO::KeyCode key)
    {
        keysOfAction[actionName] = {key};
    }

    void Input::addKeyToAction(std::string actionName, HUGO::KeyCode key)
    {
        keysOfAction[actionName].push_back(key);
    }

    void Input::removeKeyFromAction(std::string actionName, HUGO::KeyCode key)
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

    bool Input::isActionPressed(std::string action)
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

    bool Input::wasActionJustPressed(std::string action)
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

    bool Input::wasActionJustReleased(std::string action)
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

    int Input::hugoMouseInputToSDLMouseInput(HUGO::MouseCode button)
    {
        int sdlButton;

        switch(button){
        case(MOUSE_LEFT):
            sdlButton = SDL_BUTTON_LEFT;
            break;
        case(MOUSE_MIDDLE):
            sdlButton = SDL_BUTTON_MIDDLE;
            break;
        case(MOUSE_RIGHT):
            sdlButton = SDL_BUTTON_RIGHT;
            break;
        case(MOUSE_BUTTON_X1):
            sdlButton = SDL_BUTTON_X1;
            break;
        case(MOUSE_BUTTON_X2):
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
        case(UNKNOWN):
            sdlKey = SDLK_UNKNOWN;
            break;
        case(Q):
            sdlKey = SDLK_q;
            break;
        case(W):
            sdlKey = SDLK_w;
            break;
        case(E):
            sdlKey = SDLK_e;
            break;
        case(R):
            sdlKey = SDLK_r;
            break;
        case(T):
            sdlKey = SDLK_t;
            break;
        case(Y):
            sdlKey = SDLK_y;
            break;
        case(U):
            sdlKey = SDLK_u;
            break;
        case(I):
            sdlKey = SDLK_i;
            break;
        case(O):
            sdlKey = SDLK_o;
            break;
        case(P):
            sdlKey = SDLK_p;
            break;
        case(A):
            sdlKey = SDLK_a;
            break;
        case(S):
            sdlKey = SDLK_s;
            break;
        case(D):
            sdlKey = SDLK_d;
            break;
        case(F):
            sdlKey = SDLK_f;
            break;
        case(G):
            sdlKey = SDLK_g;
            break;
        case(H):
            sdlKey = SDLK_h;
            break;
        case(J):
            sdlKey = SDLK_j;
            break;
        case(K):
            sdlKey = SDLK_k;
            break;
        case(L):
            sdlKey = SDLK_l;
            break;
        case(Z):
            sdlKey = SDLK_z;
            break;
        case(X):
            sdlKey = SDLK_x;
            break;
        case(C):
            sdlKey = SDLK_c;
            break;
        case(V):
            sdlKey = SDLK_v;
            break;
        case(B):
            sdlKey = SDLK_b;
            break;
        case(N):
            sdlKey = SDLK_n;
            break;
        case(M):
            sdlKey = SDLK_m;
            break;
        case(ONE):
            sdlKey = SDLK_1;
            break;
        case(TWO):
            sdlKey = SDLK_2;
            break;
        case(THREE):
            sdlKey = SDLK_3;
            break;
        case(FOUR):
            sdlKey = SDLK_4;
            break;
        case(FIVE):
            sdlKey = SDLK_5;
            break;
        case(SIX):
            sdlKey = SDLK_6;
            break;
        case(SEVEN):
            sdlKey = SDLK_7;
            break;
        case(EIGHT):
            sdlKey = SDLK_8;
            break;
        case(NINE):
            sdlKey = SDLK_9;
            break;
        case(ZERO):
            sdlKey = SDLK_0;
            break;
        case(PANEL_ONE):
            sdlKey = SDLK_KP_1;
            break;
        case(PANEL_TWO):
            sdlKey = SDLK_KP_2;
            break;
        case(PANEL_THREE):
            sdlKey = SDLK_KP_3;
            break;
        case(PANEL_FOUR):
            sdlKey = SDLK_KP_4;
            break;
        case(PANEL_FIVE):
            sdlKey = SDLK_KP_5;
            break;
        case(PANEL_SIX):
            sdlKey = SDLK_KP_6;
            break;
        case(PANEL_SEVEN):
            sdlKey = SDLK_KP_7;
            break;
        case(PANEL_EIGHT):
            sdlKey = SDLK_KP_8;
            break;
        case(PANEL_NINE):
            sdlKey = SDLK_KP_9;
            break;
        case(PANEL_ZERO):
            sdlKey = SDLK_KP_0;
            break;
        case(PANEL_DIVIDE):
            sdlKey = SDLK_KP_DIVIDE;
            break;
        case(PANEL_MULTIPLY):
            sdlKey = SDLK_KP_MULTIPLY;
            break;
        case(PANEL_MINUS):
            sdlKey = SDLK_KP_MINUS;
            break;
        case(PANEL_PLUS):
            sdlKey = SDLK_KP_PLUS;
            break;
        case(PANEL_DECIMAL):
            sdlKey = SDLK_KP_DECIMAL;
            break;
        case(LCTRL):
            sdlKey = SDLK_LCTRL;
            break;
        case(RCTRL):
            sdlKey = SDLK_RCTRL;
            break;
        case(ALT):
            sdlKey = SDLK_LALT;
            break;
        case(ALTGR):
            sdlKey = SDLK_RALT;
            break;
        case(LMAYUS):
            sdlKey = SDLK_LSHIFT;
            break;
        case(RMAYUS):
            sdlKey = SDLK_RSHIFT;
            break;
        case(TAB):
            sdlKey = SDLK_TAB;
            break;
        case(ESC):
            sdlKey = SDLK_ESCAPE;
            break;
        case(SUPR):
            sdlKey = SDLK_DELETE;
            break;
        case(SUPER):
            sdlKey = SDLK_HOME;
            break;
        case(BACKLASH):
            sdlKey = SDLK_BACKSLASH;
            break;
        case(SLASH):
            sdlKey = SDLK_SLASH;
            break;
        case(RETURN):
            sdlKey = SDLK_RETURN;
            break;
        case(BACKSPACE):
            sdlKey = SDLK_BACKSPACE;
            break;
        case(EXCLAMATION):
            sdlKey = SDLK_EXCLAIM;
            break;
        case(DOUBLE_QUOTE):
            sdlKey = SDLK_QUOTEDBL;
            break;
        case(SIMPLE_QUOTE):
            sdlKey = SDLK_QUOTE;
            break;
        case(HASH):
            sdlKey = SDLK_HASH;
            break;
        case(PERCENT):
            sdlKey = SDLK_HASH;
            break;
        case(DOLLAR):
            sdlKey = SDLK_DOLLAR;
            break;
        case(AMPERSAND):
            sdlKey = SDLK_AMPERSAND;
            break;
        case(LEFT_PARENTHESIS):
            sdlKey = SDLK_LEFTPAREN;
            break;
        case(RIGHT_PARENTHESIS):
            sdlKey = SDLK_RIGHTPAREN;
            break;
        case(ASTERISK):
            sdlKey = SDLK_ASTERISK;
            break;
        case(PLUS):
            sdlKey = SDLK_PLUS;
            break;
        case(MINUS):
            sdlKey = SDLK_MINUS;
            break;
        case(COMMA):
            sdlKey = SDLK_COMMA;
            break;
        case(PERIOD):
            sdlKey = SDLK_PERIOD;
            break;
        case(COLON):
            sdlKey = SDLK_COLON;
            break;
        case(SEMICOLON):
            sdlKey = SDLK_SEMICOLON;
            break;
        case(LESS):
            sdlKey = SDLK_LESS;
            break;
        case(EQUALS):
            sdlKey = SDLK_EQUALS;
            break;
        case(GREATER):
            sdlKey = SDLK_GREATER;
            break;
        case(QUESTION):
            sdlKey = SDLK_QUESTION;
            break;
        case(ARROBA):
            sdlKey = SDLK_AT;
            break;
        case(LEFT_BRACKET):
            sdlKey = SDLK_LEFTBRACKET;
            break;
        case(RIGHT_BRACKET):
            sdlKey = SDLK_RIGHTBRACKET;
            break;
        case(CARET):
            sdlKey = SDLK_CARET;
            break;
        case(UNDERSCORE):
            sdlKey = SDLK_UNDERSCORE;
            break;
        case(BACKQUOTE):
            sdlKey = SDLK_BACKQUOTE;
            break;
        case(UP_ARROW):
            sdlKey = SDLK_UP;
            break;
        case(DOWN_ARROW):
            sdlKey = SDLK_DOWN;
            break;
        case(LEFT_ARROW):
            sdlKey = SDLK_LEFT;
            break;
        case(RIGHT_ARROW):
            sdlKey = SDLK_RIGHT;
            break;
        case(F1):
            sdlKey = SDLK_F1;
            break;
        case(F2):
            sdlKey = SDLK_F2;
            break;
        case(F3):
            sdlKey = SDLK_F3;
            break;
        case(F4):
            sdlKey = SDLK_F4;
            break;
        case(F5):
            sdlKey = SDLK_F5;
            break;
        case(F6):
            sdlKey = SDLK_F6;
            break;
        case(F7):
            sdlKey = SDLK_F7;
            break;
        case(F8):
            sdlKey = SDLK_F8;
            break;
        case(F9):
            sdlKey = SDLK_F9;
            break;
        case(F10):
            sdlKey = SDLK_F10;
            break;
        case(F11):
            sdlKey = SDLK_F11;
            break;
        case(F12):
            sdlKey = SDLK_F12;
            break;
        default:
            sdlKey = SDLK_UNKNOWN;
            break;
        }

        return sdlKey;
    }

}
