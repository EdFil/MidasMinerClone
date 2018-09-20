#include "RenderSystem.hpp"

#include <SDL_render.h>

#include "TransformComponent.hpp"

bool RenderSystem::initialize() {
	for(RenderComponent& component : _components) {
		component._system = this;
		component.type = ComponentType::Render;
	}
    return true;
}

void RenderSystem::releaseComponent(RenderComponent* component) {
	component->state = State::Unused;
	component->cleanup();
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
