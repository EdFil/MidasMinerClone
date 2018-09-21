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

    // Cache texture size
    SDL_QueryTexture(_texture, nullptr, nullptr, &_size.x, &_size.y);

    return true;
}

void RenderComponent::draw(SDL_Renderer* renderer) {
    const auto& position = _transformComponent->position;
    SDL_Rect destRect = {static_cast<int>(position.x), static_cast<int>(position.y), _size.x, _size.y };
    SDL_RenderCopy(renderer, _texture, nullptr, &destRect);
}

void RenderComponent::cleanup() {
	_transformComponent = nullptr;
	_texture = nullptr;
}
