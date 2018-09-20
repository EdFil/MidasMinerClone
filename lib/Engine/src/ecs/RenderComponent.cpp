//
// Created by edgar on 19-09-2018.
//

#include "RenderComponent.hpp"

#include <SDL_render.h>

#include "TransformComponent.hpp"
#include "RenderSystem.hpp"

void RenderComponent::release() {
	_system->releaseComponent(this);
}

bool RenderComponent::initialize(TransformComponent* transformComponent, SDL_Texture* texture) {
    _transformComponent = transformComponent;
    _texture = texture;
    state = State::Used;
    return true;
}

void RenderComponent::draw(SDL_Renderer* renderer) {
    auto& position = _transformComponent->position;
    position.x += 0.1;

    SDL_Rect destRect = {static_cast<int>(position.x), static_cast<int>(position.y) };
    SDL_QueryTexture(_texture, nullptr, nullptr, &destRect.w, &destRect.h);
    SDL_RenderCopy(renderer, _texture, nullptr, &destRect);
}

void RenderComponent::cleanup() {
	_transformComponent = nullptr;
	_texture = nullptr;
}
