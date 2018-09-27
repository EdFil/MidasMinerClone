#include "EventDispatcher.hpp"

#include <algorithm>

#include <SDL_events.h>
#include "ecs/ECS.hpp"

void EventDispatcher::initialize() {
	_mouseEventDelegates.reserve(k_numMaxEntities);
}


void EventDispatcher::registerForApplicationEvents(ApplicationEventDelegate* delegate) {
    _applicationEventDelegates.push_back(delegate);
}

void EventDispatcher::unregisterForApplicationEvents(ApplicationEventDelegate* delegate) {
    auto it = std::remove(_applicationEventDelegates.begin(), _applicationEventDelegates.end(), delegate);
    _applicationEventDelegates.erase(it, _applicationEventDelegates.end());
}

void EventDispatcher::registerForMouseEvents(MouseEventDelegate* delegate) {
    _mouseEventDelegates.push_back(delegate);
}

void EventDispatcher::unregisterForMouseEvents(MouseEventDelegate* delegate) {
    auto it = std::remove(_mouseEventDelegates.begin(), _mouseEventDelegates.end(), delegate);
    _mouseEventDelegates.erase(it, _mouseEventDelegates.end());
}

void EventDispatcher::update() {
    // Fetch SDL_Events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                processOnQuit();
                break;
            case SDL_MOUSEBUTTONDOWN:
                processOnMouseDown(event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                processOnMouseUp(event.button);
                break;
            case SDL_MOUSEMOTION:
                processOnMouseMotion(event.motion);
                break;
            default:
                break; // Ignore
        }
    }
}

void EventDispatcher::processOnQuit() {
    for(ApplicationEventDelegate* delegate : _applicationEventDelegates) {
        delegate->onQuit();
    }
}

void EventDispatcher::processOnMouseDown(const SDL_MouseButtonEvent& button) {
    switch (button.button) {
        case SDL_BUTTON_LEFT:
            for(MouseEventDelegate* delegate : _mouseEventDelegates) {
				if (delegate->onLeftMouseDown(button.x, button.y))
					break;
            }
            break;
        default:
            break; // Ignore
    }
}

void EventDispatcher::processOnMouseUp(const SDL_MouseButtonEvent& button) {
    switch (button.button) {
        case SDL_BUTTON_LEFT:
            for(MouseEventDelegate* delegate : _mouseEventDelegates) {
				if (delegate->onLeftMouseUp(button.x, button.y))
					break;
            }
            break;
        default:
            break; // Ignore
    }
}

void EventDispatcher::processOnMouseMotion(const SDL_MouseMotionEvent& motion) {
    for(MouseEventDelegate* delegate : _mouseEventDelegates) {
		if (delegate->onMouseMotion(motion.x, motion.y))
			break;
    }
}
