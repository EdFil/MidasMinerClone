#include "GemsComponent.hpp"

#include <SDL_rect.h>
#include <SDL_assert.h>

#include "GemsSystem.hpp"
#include "Engine.hpp"


void GemsComponent::release() {
    SDL_assert(_system != nullptr);
    _system->releaseComponent(this);
}

bool GemsComponent::initialize(GemsSystem* system, RenderComponent* renderComponent) {
    _system = system;
    _renderComponent = renderComponent;
    _isActive = false;
    state = State::Used;

    return _system != nullptr && _renderComponent != nullptr;
}

void GemsComponent::cleanup() {
    _renderComponent = nullptr;
    setIsActive(false);
}


void GemsComponent::setIsActive(bool value) {
    if(_isActive != value) {
        _isActive = value;

        if(_isActive) {
            _system->engine()->eventDispatcher()->registerForMouseEvents(this);
        } else {
            _system->engine()->eventDispatcher()->unregisterForMouseEvents(this);
        }
    }
}

void GemsComponent::onLeftMouseDown(int x, int y) {
    const glm::vec2& position = _renderComponent->transformComponent()->position();
    const glm::vec2& size = _renderComponent->size();

    SDL_Rect gemRect{static_cast<int>(position.x), static_cast<int>(position.y),
                     static_cast<int>(size.x), static_cast<int>(size.y)};
    SDL_Point mousePosition{x, y};

    if(SDL_PointInRect(&mousePosition, &gemRect)) {
        _renderComponent->setIsVisible(!_renderComponent->isVisible());
    }
}

void GemsComponent::onLeftMouseUp(int x, int y) {

}

void GemsComponent::onMouseMotion(int x, int y) {

}
