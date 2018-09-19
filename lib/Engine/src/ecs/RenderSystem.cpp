#include "RenderSystem.hpp"

bool RenderSystem::initialize() {
    return true;
}

bool RenderSystem::draw(SDL_Renderer* renderer) {
    for(RenderComponent& component : _components) {
        if(component.state == State::Used) {
            component.draw(renderer);
        }
    }

    return true;
}

RenderComponent* RenderSystem::createComponent(TransformComponent* transformComponent, SDL_Texture* texture) {
    for(RenderComponent& component : _components) {
        if(component.state == State::Unused) {
            component.initialize(transformComponent, texture);
            return &component;
        }
    }

    return nullptr;
}
