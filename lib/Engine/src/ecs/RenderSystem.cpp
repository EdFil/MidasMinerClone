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
	component->isUsed = false;
	component->cleanup();
}

bool RenderSystem::draw(SDL_Renderer* renderer) {
    for(RenderComponent& component : _components) { // TODO: ZOrdering + Don't cycle all components?
        if(component.isUsed) {
            component.draw(renderer);
        }
    }

    return true;
}

RenderComponent* RenderSystem::createComponent(TransformComponent* transformComponent, SDL_Texture* texture) {
    for(RenderComponent& component : _components) {
        if(!component.isUsed) {
            component.initialize(transformComponent, texture);
			component.isUsed = true;
            return &component;
        }
    }

    return nullptr;
}
