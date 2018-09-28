//
// Created by edgar on 19-09-2018.
//

#include "RenderComponent.hpp"

#include <SDL_assert.h>
#include <SDL_render.h>

#include "TransformComponent.hpp"
#include "RenderSystem.hpp"

void RenderComponent::release() {
	_system->releaseComponent(this);
}

bool RenderComponent::initialize(TransformComponent* transformComponent, SDL_Texture* texture) {
	SDL_assert(transformComponent != nullptr);
	_transformComponent = transformComponent;
	setTexture(texture);

    return true;
}

void RenderComponent::setTexture(SDL_Texture* texture) {
	_texture = texture;
	if(_texture != nullptr) {
		// Cache texture size
		SDL_QueryTexture(_texture, nullptr, nullptr, &_size.x, &_size.y);
	}
}

void RenderComponent::deleteTexture() {
	if(_texture != nullptr) {
		SDL_DestroyTexture(_texture);
		_texture = nullptr;
	}
}

void RenderComponent::draw(SDL_Renderer* renderer) {
    if(_isVisible && _texture != nullptr) {
        const auto &position = _transformComponent->position();
        SDL_Rect destRect = {static_cast<int>(position.x), static_cast<int>(position.y), _size.x, _size.y};
        SDL_RenderCopy(renderer, _texture, nullptr, &destRect);
    }
}

void RenderComponent::cleanup() {
	_transformComponent = nullptr;
	_texture = nullptr;
}
