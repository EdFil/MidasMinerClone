#include "GemsComponent.hpp"

#include <SDL_rect.h>
#include <SDL_assert.h>
#include <SDL_log.h>

#include "GemsSystem.hpp"
#include "Engine.hpp"
#include "TextureManager.hpp"
#include "glm/glm.hpp"

static const int k_deltaForSwipe = 10;

TextureID textureIDForGemType(const GemType gemType) {
	switch (gemType) {
		case GemType::Blue: return TextureID::Blue;
		case GemType::Green: return TextureID::Green;
		case GemType::Purple: return TextureID::Purple;
		case GemType::Red: return TextureID::Red;
		case GemType::Yellow: return TextureID::Yellow;
		default:
			SDL_assert(false);
			return TextureID::Blue;
	}
}

void GemsComponent::release() {
    SDL_assert(_system != nullptr);
    _system->releaseComponent(this);
}

bool GemsComponent::initialize(GemsSystem* system, RenderComponent* renderComponent) {
    _system = system;
    _renderComponent = renderComponent;
	_gemStatus = GemStatus::Despawned;
    
    return _system != nullptr && _renderComponent != nullptr;
}

void GemsComponent::cleanup() {
    SDL_assert(!isActive());

    _renderComponent = nullptr;
}

void GemsComponent::onAddedToBoard(const glm::vec<2, int>& index) {
    SDL_assert(_gemStatus == GemStatus::Despawned);
    SDL_assert(_boardIndex.x == -1 && _boardIndex.y == -1);

	_boardIndex = index;
	_gemStatus = GemStatus::Spawned;
	_finalPosition = _system->positionForIndex(index);
    _system->engine()->eventDispatcher()->registerForMouseEvents(this);
}

void GemsComponent::onSwap(GemsComponent& gemComponent) {
	std::swap(gemComponent._boardIndex, _boardIndex);

	_finalPosition = GemsSystem::positionForIndex(_boardIndex);
	gemComponent._finalPosition = GemsSystem::positionForIndex(gemComponent._boardIndex);

	gemComponent._gemStatus = _gemStatus = GemStatus::Swapping;
}

void GemsComponent::onSwapBackToIndex(const glm::ivec2& index) {
	_finalPosition = GemsSystem::positionForIndex(index);
	_gemStatus = GemStatus::Swapping;
}

void GemsComponent::onMovedInBoard(const glm::vec<2, int>& index) {
    SDL_assert(_gemStatus != GemStatus::Despawned && _gemStatus != GemStatus::INVALID);
    SDL_assert(_boardIndex.x != -1 && _boardIndex.y != -1);

	_gemStatus = GemStatus::Rest;
	_finalPosition = _system->positionForIndex(index);
	_boardIndex = index;
}

void GemsComponent::onRemovedFromBoard() {
    _boardIndex = { -1, -1 };
	_gemStatus = GemStatus::Despawned;
	_renderComponent->setIsVisible(false);
    _system->engine()->eventDispatcher()->unregisterForMouseEvents(this);
}

void GemsComponent::update(float delta) {
	if (_gemStatus == GemStatus::INVALID || _gemStatus == GemStatus::Despawned || _gemStatus == GemStatus::Rest)
		return;

	if (_gemStatus == GemStatus::Spawned) 
		_gemStatus = GemStatus::Falling;

	TransformComponent* transform = _renderComponent->transformComponent();
	if(_gemStatus == GemStatus::Falling) {
		if(transform->position().y < _finalPosition.y) {
			transform->setPositionY(transform->position().y + k_fallSpeed * delta);
		} else {
			transform->setPosition(_finalPosition);
			_gemStatus = GemStatus::Rest;
			_system->onFinishedFalling(this);
		}
	} else if (_gemStatus == GemStatus::Swapping || _gemStatus == GemStatus::SwappingBack) {
		// Auxiliary calculations
		const glm::vec2 movementDelta{ k_swapSpeed * delta };
		const glm::vec2 currentDirection = glm::normalize(_finalPosition - transform->position());
		const glm::vec2 positionWithDelta = transform->position() + movementDelta * currentDirection;
		const glm::vec2 directionAfterDelta = glm::normalize(_finalPosition - positionWithDelta);

		// If we changed directions, we have gone too far, set to finalPosition and finish animation
		if (std::abs(currentDirection.x - directionAfterDelta.x) > 0.1f || std::abs(currentDirection.y - directionAfterDelta.y) > 0.1f) {
			transform->setPosition(_finalPosition);

			if (_gemStatus == GemStatus::Swapping) {
				_gemStatus = GemStatus::Swapped;
				if (_swappingWith->_gemStatus == GemStatus::Swapped) {
					_system->onGemsSwapped(this, _swappingWith);
				}
			} else {
				_gemStatus = GemStatus::Rest;
			}
		} else {
			transform->setPosition(positionWithDelta);
		}
	}
}

bool GemsComponent::canBeMatchedWith(const GemType gemType) const {
	return (_gemStatus == GemStatus::Spawned || _gemStatus == GemStatus::Rest) && _gemType == gemType;
}

void GemsComponent::setGemType(GemType gemType) {
	SDL_assert(gemType != GemType::INVALID);

	_gemType = gemType;
	const auto texture = _system->engine()->textureManager()->loadTexture(textureIDForGemType(gemType));
	_renderComponent->setTexture(texture);
}

bool GemsComponent::onLeftMouseDown(int x, int y) {
    const glm::vec2& position = _renderComponent->transformComponent()->position();
    const glm::vec2& size = _renderComponent->size();

    SDL_Rect gemRect{static_cast<int>(position.x), static_cast<int>(position.y),
                     static_cast<int>(size.x), static_cast<int>(size.y)};
    SDL_Point mousePosition{x, y};

    if(SDL_PointInRect(&mousePosition, &gemRect)) {
		_mouseDownPosition = { x, y };
		_isDragging = false;
		return true;
    }

	return false;
}

bool GemsComponent::onLeftMouseUp(int x, int y) {
	if(_mouseDownPosition != glm::ivec2{-1, -1}) {
		if(!_isDragging) {
			_system->onGemClicked(this);
		} else {
			const auto translation = glm::ivec2{ x, y } -_mouseDownPosition;
			if (abs(translation.x) >= abs(translation.y)) {
				if(translation.x < -k_deltaForSwipe) {
					_system->onGemSwipedLeft(this);
				} else if(translation.x > k_deltaForSwipe) {
					_system->onGemSwipedRight(this);
				}
			} else {
				if(translation.y > -k_deltaForSwipe) {
					_system->onGemSwipedDown(this);
				} else if (translation.y < k_deltaForSwipe){
					_system->onGemSwipedUp(this);
				}
			}
		}

		_isDragging = false;
		_mouseDownPosition = { -1, -1 };
		return true;
	}

	return false;
}

bool GemsComponent::onMouseMotion(int x, int y) {
	if (_mouseDownPosition == glm::ivec2{ -1, -1 }) return false;

	if(!_isDragging && std::abs(_mouseDownPosition.x - x + _mouseDownPosition.y - y) > 5) {
		_isDragging = true;
	}

	if (_isDragging) {
		
	}

	return true;
}
