#pragma once

#include <vector>
#include <SDL_events.h>

class RenderComponent;

struct ApplicationEventDelegate {
    virtual void onQuit() = 0;
};

struct MouseEventDelegate {
    virtual bool onLeftMouseDown(int x, int y) = 0;
    virtual bool onLeftMouseUp(int x, int y) = 0;
    virtual bool onMouseMotion(int x, int y) = 0;
};

struct KeyEventDelegate {
    virtual bool onKeyDown(const SDL_Keysym& keySym) = 0;
    virtual bool onKeyUp(const SDL_Keysym& keySym) = 0;
};


class EventDispatcher {
public:
    void initialize();

    void registerForApplicationEvents(ApplicationEventDelegate* delegate);
    void unregisterForApplicationEvents(ApplicationEventDelegate* delegate);

    void registerForMouseEvents(MouseEventDelegate* delegate);
    void unregisterForMouseEvents(MouseEventDelegate* delegate);

    void registerForKeyEvents(KeyEventDelegate* delegate);
    void unregisterForKeyEvents(KeyEventDelegate* delegate);

    void update();

private:
    std::vector<ApplicationEventDelegate*> _applicationEventDelegates;
    std::vector<MouseEventDelegate*> _mouseEventDelegates;
    std::vector<KeyEventDelegate*> _keyEventDelegates;

    void processOnQuit();
    void processOnMouseDown(const SDL_MouseButtonEvent& event);
    void processOnMouseUp(const SDL_MouseButtonEvent& event);
    void processOnMouseMotion(const SDL_MouseMotionEvent& event);
    void processOnKeyDown(const SDL_KeyboardEvent& event);
    void processOnKeyUp(const SDL_KeyboardEvent& event);
};