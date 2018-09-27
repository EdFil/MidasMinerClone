#include "GemsComponent.hpp"

#include <SDL_rect.h>
#include <SDL_assert.h>
#include <SDL_log.h>

#include "GemsSystem.hpp"
#include "Engine.hpp"
#include "TextureManager.hpp"

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
	_gemStatus = GemStatus::Falling;
	_finalPosition = _system->positionForIndex(index);
    _system->engine()->eventDispatcher()->registerForMouseEvents(this);
}

void GemsComponent::onSwap(GemsComponent& gemComponent) {
	std::swap(gemComponent._boardIndex, _boardIndex);

	_finalPosition = GemsSystem::positionForIndex(_boardIndex);
	gemComponent._finalPosition = GemsSystem::positionForIndex(_boardIndex);

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
    SDL_assert(_gemStatus == GemStatus::Rest);
    SDL_assert(_boardIndex.x != -1 && _boardIndex.y != -1);

    _boardIndex = { -1, -1 };
	_gemStatus = GemStatus::Despawned;
    _system->engine()->eventDispatcher()->unregisterForMouseEvents(this);
}

void GemsComponent::update(float delta) {
	if (_gemStatus == GemStatus::INVALID || _gemStatus == GemStatus::Despawned)
		return;

	TransformComponent* transform = _renderComponent->transformComponent();

	if(_gemStatus == GemStatus::Falling) {
		if(transform->position().y <= _finalPosition.y) {
			transform->setPositionY(transform->position().y + k_fallSpeed * delta);
		} else {
			transform->setPosition(_finalPosition);
			_gemStatus = GemStatus::Rest;
		}
	} else if (_gemStatus == GemStatus::Swapping) {
		const int direction = transform->position().x > _finalPosition.x ? 1 : -1;
		const float predictedFinalPosition = transform->position().x + k_swapSpeed * delta * direction;
		if ((direction > 0 && predictedFinalPosition > _finalPosition.x) || 
			(direction < 0 && predictedFinalPosition < _finalPosition.x)) {
			transform->setPosition(_finalPosition);
			_gemStatus = GemStatus::Swapped;
			_system->onGemSwapped(this);
		} else {
			transform->setPositionX(predictedFinalPosition);
		}
	}
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
		_system->onGemClicked(this);
		return true;
    }

	return false;
}

bool GemsComponent::onLeftMouseUp(int x, int y) {
	return false;
}

bool GemsComponent::onMouseMotion(int x, int y) {
	return false;
}
