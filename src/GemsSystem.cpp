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
#include "glm/integer.hpp"

#include <Engine.hpp>
#include <TextureManager.hpp>
#include <ecs/EntitySystem.hpp>
#include <ecs/TransformSystem.hpp>
#include <ecs/RenderSystem.hpp>

static const int k_gemSize = 35;
static const int k_gemPadding = 8;
static const glm::vec2 k_startPosition = glm::vec2(320, 92);
static const int k_spawnHeight = 60;

glm::vec2 positionForIndex(const glm::vec<2, int> index) {
    int y = k_numGemsY - index.y;
    return glm::vec2(k_startPosition.x + k_gemPadding * (index.x + 1) + index.x * k_gemSize,
                     k_startPosition.y + k_gemPadding * (y + 1) + y * k_gemSize);
}

bool GemsSystem::initialize(Engine* engine) {
    _gameState = GameState::Initializing;
    _engine = engine;
	randomGenerator.seed(time(nullptr));
	_engine->textureManager()->preloadAllTextures();

    for(GemsComponent& component : _components) {
        component.type = ComponentType::Gem;
    }

    const size_t numGems = k_numGemsX * k_numGemsY;
    _waiting.reserve(numGems);
	_dirty.reserve(numGems);
	_swapedEntities.reserve(2);

    for(size_t i = 0; i < numGems; i++) {
        auto transform = engine->transformSystem()->createComponent();

        auto render = engine->renderSystem()->createComponent(transform, nullptr);
        render->setIsVisible(false);

        auto gemsComponent = createComponent(render);

        auto entity = engine->entitySystem()->createEntity();
        entity->addComponent(transform);
        entity->addComponent(render);
        entity->addComponent(gemsComponent);

        _waiting.push_back(entity);
    }

    return false;
}

void GemsSystem::update(float delta) {
	static int i = 0;
	if (i++ % 60 != 0) return;

	if (!_swapedEntities.empty()) {
		bool wasSwapValid = false;
		for (Entity* entity : _swapedEntities) {
			GemsComponent* gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (gemComponent->_isActive) {
				NewBackToBackCount count = theNewBackToBackCount(gemComponent->index(), gemComponent->gemType());
				if (count.numHorizontalGems > 2) {
					wasSwapValid = true;
					for (int i = 0; i < count.numHorizontalGems; i++)
						removeEntity(count.horizontalGems[i]->index());
				}
				if (count.numVerticalGems > 2) {
					wasSwapValid = true;
					for (int i = 0; i < count.numVerticalGems; i++)
						removeEntity(count.verticalGems[i]->index());
				}
			}
		}

		if (!wasSwapValid) {
			cancelGemSwap();
		}

		_swapedEntities.clear();
		return;
	}

	if (!_dirty.empty()) {
		for (Entity* entity : _dirty) {
			auto* gem = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (gem->_isActive) {
				const NewBackToBackCount backToBackGems = theNewBackToBackCount(gem->index(), gem->gemType());
				if (backToBackGems.numHorizontalGems >= 3) {
					for (int i = 0; i < backToBackGems.numHorizontalGems; i++) {
						if (backToBackGems.horizontalGems[i]->_isActive)
							removeEntity(backToBackGems.horizontalGems[i]->index());
					}
				}
				if (backToBackGems.numVerticalGems >= 3) {
					for (int i = 0; i < backToBackGems.numVerticalGems; i++) {
						if (backToBackGems.verticalGems[i]->_isActive)
							removeEntity(backToBackGems.verticalGems[i]->index());
					}
				}
			}
		}

		_dirty.clear();
		return;
	}

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

			moveEntityFromTo({x, occupiedIndex}, {x, vacantIndex});
			vacantIndex++;
		}

		// If by this point vacantIndex is not on the top, we need to spawn in new gems
		for (; vacantIndex < k_numGemsY; vacantIndex++) {
			createNewRandomGemOnIndex({x, vacantIndex});
		}
    }
}

void GemsSystem::moveEntityFromTo(const glm::ivec2& fromIndex, const glm::ivec2& toIndex) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Moving Gem from [%d][%d] to [%d][%d]", fromIndex.x, fromIndex.y, toIndex.x, toIndex.y);

	std::swap(_board[fromIndex.x][fromIndex.y], _board[toIndex.x][toIndex.y]);
	Entity* toEntity = _board[toIndex.x][toIndex.y];

	auto* transform = static_cast<TransformComponent*>(toEntity->getComponentWithType(ComponentType::Transform));
	transform->setPosition(positionForIndex(toIndex));

	auto* gem = static_cast<GemsComponent*>(toEntity->getComponentWithType(ComponentType::Gem));
	gem->onMovedInBoard(toIndex);
	updateGemPointers(gem);

	_dirty.push_back(toEntity);
}

void GemsSystem::swapGems(GemsComponent* firstComponent, GemsComponent* secondComponent) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Trying to swap [%d][%d] with [%d][%d]", firstComponent->index().x, firstComponent->index().y, secondComponent->index().x, secondComponent->index().y);

	Entity* firstEntity = firstComponent->entity();
	Entity* secondEntity = secondComponent->entity();
	TransformComponent* firstTransform = static_cast<TransformComponent*>(firstEntity->getComponentWithType(ComponentType::Transform));
	TransformComponent* secondTransform = static_cast<TransformComponent*>(secondEntity->getComponentWithType(ComponentType::Transform));

	// Swap positions
	const auto firstPosition = firstTransform->position();
	firstTransform->setPosition(secondTransform->position());
	secondTransform->setPosition(firstPosition);

	// Swap on the board
	std::swap(_board[firstComponent->index().x][firstComponent->index().y], _board[secondComponent->index().x][secondComponent->index().y]);

	// Swap indexes
	const auto firstIndex = firstComponent->index();
	firstComponent->onMovedInBoard(secondComponent->index());
	secondComponent->onMovedInBoard(firstIndex);


	updateGemPointers(firstComponent);
	updateGemPointers(secondComponent);


	// Save entities to check if the swap was valid
	_swapedEntities.push_back(firstEntity);
	_swapedEntities.push_back(secondEntity);
}

void GemsSystem::cancelGemSwap() {
	SDL_assert(!_swapedEntities.empty());

	Entity* firstEntity = _swapedEntities[0];
	Entity* secondEntity = _swapedEntities[1];
	GemsComponent* firstComponent = static_cast<GemsComponent*>(firstEntity->getComponentWithType(ComponentType::Gem));
	GemsComponent* secondComponent = static_cast<GemsComponent*>(secondEntity->getComponentWithType(ComponentType::Gem));
	TransformComponent* firstTransform = static_cast<TransformComponent*>(firstEntity->getComponentWithType(ComponentType::Transform));
	TransformComponent* secondTransform = static_cast<TransformComponent*>(secondEntity->getComponentWithType(ComponentType::Transform));

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Failed to swap, swapping back");

	// Swap positions
	const auto firstPosition = firstTransform->position();
	firstTransform->setPosition(secondTransform->position());
	secondTransform->setPosition(firstPosition);

	// Swap on the board
	std::swap(_board[firstComponent->index().x][firstComponent->index().y], _board[secondComponent->index().x][secondComponent->index().y]);

	// Swap indexes
	const auto firstIndex = firstComponent->index();
	firstComponent->onMovedInBoard(secondComponent->index());
	secondComponent->onMovedInBoard(firstIndex);

	updateGemPointers(firstComponent);
	updateGemPointers(secondComponent);
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

void GemsSystem::onGemClicked(GemsComponent* gemComponent) {
	if(_selectedGem == nullptr) {
		_selectedGem = gemComponent;
	} else {
		const glm::ivec2 gemDistance = glm::abs(_selectedGem->index() - gemComponent->index());
		if(gemDistance.x + gemDistance.y == 1)
			swapGems(gemComponent, _selectedGem);
		
		_selectedGem = nullptr;
	}
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

void GemsSystem::createNewRandomGemOnIndex(const glm::vec<2, int>& index) {
	SDL_assert(!_waiting.empty());
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Spawn a new gem to [%d][%d]", index.x, index.y);

	Entity* entityToBeSpawned = _waiting.back();
	_waiting.pop_back();

	auto transform = static_cast<TransformComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Transform));
	transform->setPosition(positionForIndex(index));

	auto render = static_cast<RenderComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Render));
	render->setIsVisible(true);

	auto gem = static_cast<GemsComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Gem));
	gem->setGemType(randomPossibleGemForIndex(index));
	gem->onAddedToBoard(index);

	updateGemPointers(gem);

	_board[index.x][index.y] = entityToBeSpawned;
}

BackToBackCount GemsSystem::backToBackCountOnIndex(const glm::ivec2& index, const GemType gemType) {
	BackToBackCount count;
	std::array<GemsComponent*, k_numMaxGemsComponents> checkedGems = { nullptr };
	unsigned numCheckedGems = 0;
	std::array<GemsComponent*, k_numMaxGemsComponents> gemsToCheck = { nullptr };
	unsigned numGemsToCheck = 0;

	Entity* entity = _board[index.x][index.y];
	if (entity != nullptr) {
		gemsToCheck[numGemsToCheck++] = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
	} else {
		if (index.x > 0 && _board[index.x - 1][index.y] != nullptr) gemsToCheck[numGemsToCheck++] = static_cast<GemsComponent*>(_board[index.x - 1][index.y]->getComponentWithType(ComponentType::Gem));
		if (index.x < k_numGemsX && _board[index.x + 1][index.y] != nullptr) gemsToCheck[numGemsToCheck++] = static_cast<GemsComponent*>(_board[index.x + 1][index.y]->getComponentWithType(ComponentType::Gem));
		if (index.y > 0 && _board[index.x - 1][index.y - 1] != nullptr) gemsToCheck[numGemsToCheck++] = static_cast<GemsComponent*>(_board[index.x][index.y - 1]->getComponentWithType(ComponentType::Gem));
		if (index.y < k_numGemsY && _board[index.x][index.y + 1] != nullptr) gemsToCheck[numGemsToCheck++] = static_cast<GemsComponent*>(_board[index.x][index.y + 1]->getComponentWithType(ComponentType::Gem));
	}

	while(numGemsToCheck > 0) {
		GemsComponent* currentGem = gemsToCheck[--numGemsToCheck];
		if(std::find(checkedGems.cbegin(), checkedGems.cend(), currentGem) != checkedGems.cend()) {
			continue;
		}
		checkedGems[numCheckedGems++] = currentGem;

		if(currentGem->gemType() == gemType) {
			count.gems[count.numGems++] = currentGem;
			if (currentGem->leftGem() != nullptr) gemsToCheck[numGemsToCheck++] = currentGem->leftGem();
			if (currentGem->rightGem() != nullptr) gemsToCheck[numGemsToCheck++] = currentGem->rightGem();
			if (currentGem->upGem() != nullptr) gemsToCheck[numGemsToCheck++] = currentGem->upGem();
			if (currentGem->downGem() != nullptr) gemsToCheck[numGemsToCheck++] = currentGem->downGem();
		}
	}
	
	return count;
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
			if (gemComponent->gemType() != gemType) break;
			count.horizontalGems[count.numHorizontalGems++] = gemComponent;

		}
	}

	// Count right
	for (int x = index.x + 1; x < k_numGemsX; x++) {
		if (Entity* entity = _board[x][index.y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (gemComponent->gemType() != gemType) break;
			count.horizontalGems[count.numHorizontalGems++] = gemComponent;

		}
	}

	// Count down
	for (int y = index.y - 1; y >= 0; y--) {
		if (Entity* entity = _board[index.x][y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (gemComponent->gemType() != gemType) break;
			count.verticalGems[count.numVerticalGems++] = gemComponent;

		}
	}

	// Count up
	for (int y = index.y + 1; y < k_numGemsY; y++) {
		if (Entity* entity = _board[index.x][y]) {
			const auto gemComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			if (gemComponent->gemType() != gemType) break;
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

	SDL_assert(false); // No possible Gem?
	return GemType::Green;
}

void GemsSystem::updateGemPointers(GemsComponent* gemComponent) {
	SDL_assert(gemComponent != nullptr);
	const auto& index = gemComponent->index();

	if (index.x - 1 > 0) {
		Entity* entity = _board[index.x - 1][index.y];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_right = gemComponent;
			gemComponent->_left = component;
		}
	} else {
		gemComponent->_left = nullptr;
	}

	if (index.x + 1 < k_numGemsX) {
		Entity* entity = _board[index.x + 1][index.y];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_left = gemComponent;
			gemComponent->_right = component;
		}
	} else {
		gemComponent->_right = nullptr;
	}

	if (index.y - 1 > 0) {
		Entity* entity = _board[index.x][index.y - 1];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_up = gemComponent;
			gemComponent->_down = component;
		}
	} else {
		gemComponent->_down = nullptr;
	}

	if (index.y + 1 < k_numGemsY) {
		Entity* entity = _board[index.x][index.y + 1];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_down = gemComponent;
			gemComponent->_up = component;
		}
	} else {
		gemComponent->_up = nullptr;
	}
}
