//
// Created by edgar on 20-09-2018.
//

#include "GemsSystem.hpp"

#include <algorithm>    // std::shuffle
#include <random>
#include <ctime>

#include <SDL_render.h>
#include <SDL_log.h>
#include <SDL_assert.h>
#include <glm/integer.hpp>

#include <Engine.hpp>
#include <TextureManager.hpp>
#include <ecs/EntitySystem.hpp>
#include <ecs/TransformSystem.hpp>
#include <ecs/RenderSystem.hpp>

static const int k_gemSize = 35;
static const int k_gemPadding = 8;
static const glm::vec2 k_startPosition = glm::vec2(320, 92);
static const glm::vec2 k_timePosition = glm::vec2(90, 430);
static const float k_spawnHeight = k_startPosition.y - k_gemSize - k_gemPadding;

bool GemsSystem::initialize(Engine* engine) {
    _engine = engine;
	randomGenerator.seed(time(nullptr));

	// TODO: Try to assign this at compile time to avoid this loop
	for (GemsComponent& component : _components) {
		component.type = ComponentType::Gem;
	}

	// Allocate all the necessary memory
    const size_t numGems = k_numGemsX * k_numGemsY;
    _waiting.reserve(numGems);
	_gemsThatFell.reserve(numGems);
	_swappedGems.reserve(2); // We will probably not need to keep track of more than 4 swaps

	// Create all the entities
    for(size_t i = 0; i < numGems; i++) {
	    const auto transform = engine->transformSystem()->createComponent();

        auto render = engine->renderSystem()->createComponent(transform, nullptr);
        render->setIsVisible(false);

	    const auto gemsComponent = createComponent(render);

        auto entity = engine->entitySystem()->createEntity();
        entity->addComponent(transform);
        entity->addComponent(render);
        entity->addComponent(gemsComponent);

        _waiting.push_back(entity);
    }

	// Create the cross entity that appears when you select a gem
	auto transform = engine->transformSystem()->createComponent();
	auto render = engine->renderSystem()->createComponent(transform, _engine->textureManager()->loadTexture(TextureID::Cross));
	render->setIsVisible(false);
	_selectedGemCross = engine->entitySystem()->createEntity();
	_selectedGemCross->addComponent(transform);
	_selectedGemCross->addComponent(render);

    return false;
}

void GemsSystem::cleanup() {
    for(GemsComponent& component : _components) {
        component.cleanup();
    }
}

void GemsSystem::clearBoard() {
    for(GemsComponent& component : _components) {
        component.onRemovedFromBoard();
        _waiting.push_back(component.entity());
    }

    for(int x = 0; x < k_numGemsX; x++)
        for (int y = 0; y < k_numGemsY; y++)
            _board[x][y] = nullptr;
}

void GemsSystem::setIsClickable(bool isClickable) {
	_isClickable = isClickable;
}

bool GemsSystem::onKeyDown(const SDL_Keysym& keySym) { /* TODO: REMOVE */ }

bool GemsSystem::onKeyUp(const SDL_Keysym& keySym) { /* Do nothing */ }

void GemsSystem::update(float delta) {
	memset(_currentFrameSpawnOffset, 0, sizeof(_currentFrameSpawnOffset));

	// Check if some some finished swap animations
	if(!_swappedGems.empty()) {
		for (auto& swapPair : _swappedGems) {
			bool couldSwap1 = tryChainDelete(swapPair.first);
			bool couldSwap2 = tryChainDelete(swapPair.second);
			bool couldSwap = couldSwap1 || couldSwap2;

			if(!couldSwap) {
				cancelGemSwap(swapPair);
			} else {
				if (swapPair.first->isActive()) swapPair.first->_gemStatus = GemStatus::Rest;
				if (swapPair.second->isActive()) swapPair.second->_gemStatus = GemStatus::Rest;
			}
		}

		_swappedGems.clear();
	}

	// Check if some gem fell and need to 
	if (!_gemsThatFell.empty()) {
		for (Entity* entity : _gemsThatFell) {
			tryChainDelete(static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem)));
		}

		_gemsThatFell.clear();
	}

	for (auto& component : _components) {
		component.update(delta);
	}

	// TODO: Don't check the entire board all frames, use callbacks/dirty flags
    for(int x = 0; x < k_numGemsX; x++) {
        // Start going from bottom to top to find a vacant position
        int vacantIndex = 0;
        for(; vacantIndex < k_numGemsY && _board[x][vacantIndex] != nullptr; vacantIndex++);

        // We have reached the top, nothing to do in this column
        if(vacantIndex == k_numGemsY)
            continue;

        // Find the next place with a gem
        int occupiedIndex = vacantIndex;
        for(; occupiedIndex < k_numGemsY && _board[x][occupiedIndex] == nullptr; occupiedIndex++);

		// If we didn't reach the top, means we found a hole!! Make things fall!
		for (; occupiedIndex < k_numGemsY; occupiedIndex++) {
			if(_board[x][occupiedIndex] == nullptr) continue; // If we have powers, it's possible to have multiple holes

			makeGemFallFromTo({x, occupiedIndex}, {x, vacantIndex});
			vacantIndex++;
		}

		// If by this point vacantIndex is not on the top, we need to spawn in new gems
		for (; vacantIndex < k_numGemsY; vacantIndex++) {
			createNewRandomGemOnIndex({x, vacantIndex});
		}
    }
}

glm::vec2 GemsSystem::positionForIndex(const glm::ivec2& index) {
	int y = k_numGemsY - 1 - index.y;
	return glm::vec2(k_startPosition.x + k_gemPadding * (index.x + 1) + index.x * k_gemSize,
					 k_startPosition.y + k_gemPadding * (y + 1) + y * k_gemSize);
}

void GemsSystem::makeGemFallFromTo(const glm::ivec2& fromIndex, const glm::ivec2& toIndex) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Moving Gem from [%d][%d] to [%d][%d]", fromIndex.x, fromIndex.y, toIndex.x, toIndex.y);

	std::swap(_board[fromIndex.x][fromIndex.y], _board[toIndex.x][toIndex.y]);
	Entity* toEntity = _board[toIndex.x][toIndex.y];

	auto* gem = static_cast<GemsComponent*>(toEntity->getComponentWithType(ComponentType::Gem));
	gem->_boardIndex = toIndex;
	gem->_gemStatus = GemStatus::Falling;
	gem->_finalPosition = positionForIndex(toIndex);
}

bool GemsSystem::trySwapGem(GemsComponent* gemComponent, const glm::ivec2& index) {
	if (!gemComponent->canSwap() || index.x < 0 || index.y < 0 || index.x >= k_numGemsX || index.y >= k_numGemsY)
		return false;

	Entity* otherEntity = _board[index.x][index.y];
	if (otherEntity != nullptr) {
		const auto otherGem = static_cast<GemsComponent*>(otherEntity->getComponentWithType(ComponentType::Gem));
		if (otherGem->canSwap()) {
			swapGems(gemComponent, otherGem);
			return true;
		}
	}

	return false;
}

void GemsSystem::swapGems(GemsComponent* firstComponent, GemsComponent* secondComponent) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Trying to swap [%d][%d] with [%d][%d]", firstComponent->index().x, firstComponent->index().y, secondComponent->index().x, secondComponent->index().y);

	std::swap(_board[firstComponent->index().x][firstComponent->index().y], _board[secondComponent->index().x][secondComponent->index().y]);
	std::swap(firstComponent->_boardIndex, secondComponent->_boardIndex);
	firstComponent->_gemStatus = secondComponent->_gemStatus = GemStatus::Swapping;
	firstComponent->_finalPosition = positionForIndex(firstComponent->_boardIndex);
	secondComponent->_finalPosition = positionForIndex(secondComponent->_boardIndex);
	firstComponent->_swappingWith = secondComponent;
	secondComponent->_swappingWith = firstComponent;

	// TODO: UNSELECT GEM
}

void GemsSystem::cancelGemSwap(const std::pair<GemsComponent*, GemsComponent*>& swapPair) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Failed to swap, swapping back");

	GemsComponent* firstComponent = swapPair.first;
	GemsComponent* secondComponent = swapPair.second;

	std::swap(_board[firstComponent->index().x][firstComponent->index().y], _board[secondComponent->index().x][secondComponent->index().y]);
	std::swap(firstComponent->_boardIndex, secondComponent->_boardIndex);
	firstComponent->_gemStatus = secondComponent->_gemStatus = GemStatus::SwappingBack;
	firstComponent->_finalPosition = positionForIndex(firstComponent->_boardIndex);
	secondComponent->_finalPosition = positionForIndex(secondComponent->_boardIndex);
	firstComponent->_swappingWith = secondComponent;
	secondComponent->_swappingWith = firstComponent;
}

bool GemsSystem::tryChainDelete(GemsComponent* component) {
	if (!component->canBeRemoved()) return false;

	const NewBackToBackCount count = theNewBackToBackCount(component->index(), component->gemType());
	bool couldDelete = false;
	if (count.numHorizontalGems >= 3) {
		couldDelete = true;
		for (int i = 0; i < count.numHorizontalGems; i++) {
			if (count.horizontalGems[i]->canBeRemoved())
				removeEntity(count.horizontalGems[i]->index());
		}
	}
	if (count.numVerticalGems >= 3) {
		couldDelete = true;
		for (int i = 0; i < count.numVerticalGems; i++) {
			if (count.verticalGems[i]->canBeRemoved())
				removeEntity(count.verticalGems[i]->index());
		}
	}

	return couldDelete;
}

void GemsSystem::removeEntity(const glm::ivec2 index) {
	if (index.x == -1 && index.y == -1) return;

	Entity* entity = _board[index.x][index.y];

	auto render = static_cast<RenderComponent*>(entity->getComponentWithType(ComponentType::Render));
	render->setIsVisible(false);

	auto gem = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
	gem->onRemovedFromBoard();
	
	_waiting.push_back(entity);
	_board[index.x][index.y] = nullptr;
}


bool GemsSystem::areAllGemsResting() const {
    for(const GemsComponent& gemComponent : _components) {
        if(gemComponent.gemStatus() != GemStatus::Rest)
            return false;
    }

    return true;
}

void GemsSystem::onGemClicked(GemsComponent* gemComponent) {
	if(!_isClickable) return;

	if (gemComponent->canSwap()) {
	    // TODO: Extract this into a method to "selected gem + swipe gem" bug
		if (_selectedGem == nullptr) {
			_selectedGem = gemComponent;

			auto transform = static_cast<TransformComponent*>(_selectedGemCross->getComponentWithType(ComponentType::Transform));
			transform->setPosition(gemComponent->_renderComponent->transformComponent()->position());

			auto render = static_cast<RenderComponent*>(_selectedGemCross->getComponentWithType(ComponentType::Render));
			render->setIsVisible(true);
		}
		else {
			const glm::ivec2 gemDistance = glm::abs(_selectedGem->index() - gemComponent->index());
			if (gemDistance.x + gemDistance.y == 1)
				swapGems(gemComponent, _selectedGem);

			_selectedGem = nullptr;

			auto render = static_cast<RenderComponent*>(_selectedGemCross->getComponentWithType(ComponentType::Render));
			render->setIsVisible(false);
		}
	} else {
		
	}
}

void GemsSystem::onGemSwipedLeft(GemsComponent* gemComponent) {
	if(!_isClickable) return;

	trySwapGem(gemComponent, glm::ivec2(gemComponent->index().x - 1, gemComponent->index().y));
}

void GemsSystem::onGemSwipedRight(GemsComponent* gemComponent) {
	if(!_isClickable) return;

	trySwapGem(gemComponent, glm::ivec2(gemComponent->index().x + 1, gemComponent->index().y));
}

void GemsSystem::onGemSwipedUp(GemsComponent* gemComponent) {
	if(!_isClickable) return;

	trySwapGem(gemComponent, glm::ivec2(gemComponent->index().x, gemComponent->index().y + 1));
}

void GemsSystem::onGemSwipedDown(GemsComponent* gemComponent) {
	if(!_isClickable) return;

	trySwapGem(gemComponent, glm::ivec2(gemComponent->index().x, gemComponent->index().y - 1));
}

void GemsSystem::onGemsSwapped(GemsComponent* firstComponent, GemsComponent* secondComponent) {
	_swappedGems.emplace_back(firstComponent, secondComponent);
}

void GemsSystem::onFinishedFalling(GemsComponent* gemComponent) {
	_gemsThatFell.emplace_back(gemComponent->entity());
}


GemsComponent *GemsSystem::createComponent(RenderComponent* renderComponent) {
    for(GemsComponent& component : _components) {
        if(!component.isUsed) {
            component.initialize(this, renderComponent);
			component.isUsed = true;
            return &component;
        }
    }

    return nullptr;
}

void GemsSystem::releaseComponent(GemsComponent *component) {
    component->isUsed = false;
    component->cleanup();
}

void GemsSystem::createNewRandomGemOnIndex(const glm::ivec2& index) {
	SDL_assert(!_waiting.empty());
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Spawn a new gem to [%d][%d]", index.x, index.y);

	Entity* entityToBeSpawned = _waiting.back();

	auto transform = static_cast<TransformComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Transform));
	transform->setPosition({ positionForIndex(index).x, k_spawnHeight - _currentFrameSpawnOffset[index.x] });
	_currentFrameSpawnOffset[index.x] += k_gemSize + k_gemPadding;

	auto render = static_cast<RenderComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Render));
	render->setIsVisible(true);

	auto gem = static_cast<GemsComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Gem));
	gem->setGemType(randomPossibleGemForIndex(index));
	gem->onAddedToBoard(index);

	_board[index.x][index.y] = entityToBeSpawned;
	_waiting.pop_back();
}

NewBackToBackCount GemsSystem::theNewBackToBackCount(const glm::ivec2& index, const GemType gemType) {
	NewBackToBackCount count;

	if (Entity* entity = _board[index.x][index.y]) {
		const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));

		// Count this index for both horizontal and vertical because this is the root
		count.horizontalGems[count.numHorizontalGems++] = gemComponent;
		count.verticalGems[count.numVerticalGems++] = gemComponent;
	}

	// Count left
	for (int x = index.x - 1 ; x >= 0; x--) {
		if (Entity* entity = _board[x][index.y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (!gemComponent->canBeMatchedWith(gemType)) break;
			SDL_assert(gemComponent->gemStatus() == GemStatus::Rest || gemComponent->gemStatus() == GemStatus::Spawned);
			count.horizontalGems[count.numHorizontalGems++] = gemComponent;

		}
	}

	// Count right
	for (int x = index.x + 1; x < k_numGemsX; x++) {
		if (Entity* entity = _board[x][index.y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (!gemComponent->canBeMatchedWith(gemType)) break;
			SDL_assert(gemComponent->gemStatus() == GemStatus::Rest || gemComponent->gemStatus() == GemStatus::Spawned);
			count.horizontalGems[count.numHorizontalGems++] = gemComponent;

		}
	}

	// Count down
	for (int y = index.y - 1; y >= 0; y--) {
		if (Entity* entity = _board[index.x][y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (!gemComponent->canBeMatchedWith(gemType)) break;
			SDL_assert(gemComponent->gemStatus() == GemStatus::Rest || gemComponent->gemStatus() == GemStatus::Spawned);
			count.verticalGems[count.numVerticalGems++] = gemComponent;

		}
	}

	// Count up
	for (int y = index.y + 1; y < k_numGemsY; y++) {
		if (Entity* entity = _board[index.x][y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (!gemComponent->canBeMatchedWith(gemType)) break;
			SDL_assert(gemComponent->gemStatus() == GemStatus::Rest || gemComponent->gemStatus() == GemStatus::Spawned);
			count.verticalGems[count.numVerticalGems++] = gemComponent;

		}
	}

	return count;
}


GemType GemsSystem::randomPossibleGemForIndex(const glm::vec<2, int>& index) {
	// Generate our randomized Gem picking order
	GemType possibleGems[static_cast<int>(GemType::COUNT)];
	for(int i = 0; i < static_cast<int>(GemType::COUNT); i++) {
		possibleGems[i] = static_cast<GemType>(i);
	}

	std::shuffle(std::begin(possibleGems), std::end(possibleGems), randomGenerator);

	// Select a valid gem for this position
	for(GemType gemType : possibleGems) {
		NewBackToBackCount backToBackCount = theNewBackToBackCount(index, gemType);
		if (backToBackCount.numVerticalGems < 2 && backToBackCount.numHorizontalGems < 2) {
			return gemType;
		}
	}

	//SDL_assert(false); // No possible Gem?
	return possibleGems[0];
}
