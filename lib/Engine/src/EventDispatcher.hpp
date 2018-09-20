#pragma once

#include <vector>
#include <SDL_events.h>

class RenderComponent;

class ApplicationEventDelegate {
public:
    virtual void onQuit() = 0;
};

class MouseEventDelegate {
public:
    virtual void onLeftMouseDown(int x, int y) = 0;
    virtual void onLeftMouseUp(int x, int y) = 0;
    virtual void onMouseMotion(int x, int y) = 0;
};

class EventDispatcher {
public:
    void initialize();

    void registerForApplicationEvents(ApplicationEventDelegate* delegate);
    void unregisterForApplicationEvents(ApplicationEventDelegate* delegate);

    void registerForMouseEvents(MouseEventDelegate* delegate);
    void unregisterForMouseEvents(MouseEventDelegate* delegate);

    void update();

private:
    std::vector<ApplicationEventDelegate*> _applicationEventDelegates;
    std::vector<MouseEventDelegate*> _mouseEventDelegates;

    void processOnQuit();
    void processOnMouseDown(const SDL_MouseButtonEvent& event);
    void processOnMouseUp(const SDL_MouseButtonEvent& event);
    void processOnMouseMotion(const SDL_MouseMotionEvent& event);
};