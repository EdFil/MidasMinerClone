//
// Created by edgar on 20-09-2018.
//

#include "GemsSystem.hpp"

#include <algorithm>    // std::shuffle
#include <random>

#include <SDL_render.h>
#include <SDL_log.h>
#include <SDL_assert.h>

#include "Engine.hpp"
#include "EntitySystem.hpp"
#include "TransformSystem.hpp"
#include "RenderSystem.hpp"
#include "TextureManager.hpp"
#include <ctime>

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


		if(!_dirty.empty()) {
			for(Entity* entity : _dirty) {
				auto* gem = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
				if (gem->_isActive) {
					BackToBackCount backToBackGems = backToBackCountOnIndex(gem->index(), gem->gemType());
					if (backToBackGems.numGems >= 3) {
						for (int i = 0; i < backToBackGems.numGems; i++) {
							if (backToBackGems.gems[i]->_isActive)
								removeEntity(backToBackGems.gems[i]->index());
						}
					}
				}
			}

			_dirty.clear();
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

void GemsSystem::removeEntity(const glm::ivec2 index) {
	Entity* entity = _board[index.x][index.y];

	auto render = static_cast<RenderComponent*>(entity->getComponentWithType(ComponentType::Render));
	render->setIsVisible(false);

	auto gem = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
	gem->onRemovedFromBoard();
	
	_waiting.push_back(entity);
	_board[index.x][index.y] = nullptr;
}

GemsComponent *GemsSystem::createComponent(RenderComponent* renderComponent) {
    for(GemsComponent& component : _components) {
        if(component.state == State::Unused) {
            component.initialize(this, renderComponent);
            return &component;
        }
    }

    return nullptr;
}

void GemsSystem::releaseComponent(GemsComponent *component) {
    component->state = State::Unused;
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

GemType GemsSystem::randomPossibleGemForIndex(const glm::vec<2, int>& index) {
	// Generate our randomized Gem picking order
	GemType possibleGems[static_cast<int>(GemType::COUNT)];
	for(int i = 0; i < static_cast<int>(GemType::COUNT); i++) {
		possibleGems[i] = static_cast<GemType>(i);
	}

	std::shuffle(std::begin(possibleGems), std::end(possibleGems), randomGenerator);

	// Select a valid gem for this position
	for(GemType gemType : possibleGems) {
		BackToBackCount backToBackCount = backToBackCountOnIndex(index, gemType);
		if (backToBackCount.numGems < 2) {
			return gemType;
		}
	}

	SDL_assert(false); // No possible Gem?
	return GemType::Green;
}

void GemsSystem::updateGemPointers(GemsComponent* gemComponent) {
	SDL_assert(gemComponent != nullptr);
	const auto& index = gemComponent->index();

	if (index.x > 0) {
		Entity* entity = _board[index.x - 1][index.y];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_right = gemComponent;
			gemComponent->_left = component;
		}
	} else {
		gemComponent->_left = nullptr;
	}

	if (index.x < k_numGemsX) {
		Entity* entity = _board[index.x + 1][index.y];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_left = gemComponent;
			gemComponent->_right = component;
		}
	} else {
		gemComponent->_right = nullptr;
	}

	if (index.y > 0) {
		Entity* entity = _board[index.x][index.y - 1];
		if (entity != nullptr) {
			auto component = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
			component->_up = gemComponent;
			gemComponent->_down = component;
		}
	} else {
		gemComponent->_down = nullptr;
	}

	if (index.y > 0) {
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
