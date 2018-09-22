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

static const int k_numGemsX = 8;
static const int k_numGemsY = 8;
static const int k_gemSize = 35;
static const int k_gemPadding = 8;
static const glm::vec2 k_startPosition = glm::vec2(320, 92);
static const int k_spawnHeight = 60;

glm::vec2 positionForIndex(int x, int y) {
    y = k_numGemsY - y;
    return glm::vec2(k_startPosition.x + k_gemPadding * (x + 1) + x * k_gemSize,
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
	//if(_gameState == GameState::Initializing) {
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

            if(occupiedIndex != k_numGemsY) {
                // We didn't reach the top. Found a hole!! Make things fall!
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Make gem from [%d][%d] fall to [%d][%d]", x, occupiedIndex, x, vacantIndex);
				moveEntityFromTo(x, occupiedIndex, x, vacantIndex);

            } else {
                // Reached the top! Need to spawn new gems!
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Spawn a new gem to [%d][%d]", x, vacantIndex);
                SDL_assert(!_waiting.empty());

                Entity* entityToBeSpawned = _waiting.back();
                _waiting.pop_back();

                auto transform = static_cast<TransformComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Transform));
                transform->setPosition(positionForIndex(x, vacantIndex));

                auto render = static_cast<RenderComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Render));
				render->setTexture(_engine->textureManager()->getRandomGemTexture());
            	render->setIsVisible(true);

                auto gem = static_cast<GemsComponent*>(entityToBeSpawned->getComponentWithType(ComponentType::Gem));
				gem->setGemType(randomPossibleGemForIndex({x, vacantIndex}));
            	gem->setIsActive(true);
				gem->onAddedToBoard(x, vacantIndex);

                _board[x][vacantIndex] = entityToBeSpawned;
            }

        }
    //}
}

void GemsSystem::moveEntityFromTo(int fromX, int fromY, int toX, int toY) {
	std::swap(_board[toX][toY], _board[fromX][fromY]);
	auto* transform = static_cast<TransformComponent*>(_board[toX][toY]->getComponentWithType(ComponentType::Transform));
	transform->setPosition(positionForIndex(toX, toY));
	auto* gem = static_cast<GemsComponent*>(_board[toX][toY]->getComponentWithType(ComponentType::Gem));
	gem->onMovedInBoard(toX, toY);
}

void GemsSystem::removeEntity(int x, int y) {
	Entity* entity = _board[x][y];
	_board[x][y] = nullptr;

	auto render = static_cast<RenderComponent*>(entity->getComponentWithType(ComponentType::Render));
	render->setIsVisible(false);

	auto gem = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
	gem->setIsActive(false);
	gem->onRemovedFromBoard();
	
	_waiting.push_back(entity);
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

BackToBackCount GemsSystem::backToBackCountOnIndex(const glm::vec<2, int> index, const GemType gemType) {
	BackToBackCount count{ 0, 0, 0, 0 };

	// Search left
	for(int x = index.x - 1; x >= 0; x--) {
		Entity* entity = _board[x][index.y];
		if(entity == nullptr) {
			break;
		}
		const auto gemsComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
		if(gemsComponent->gemType() != gemType) {
			break;
		}

		count.left++;
	}

	// Search right
	for (int x = index.x + 1; x < k_numGemsX; x++) {
		Entity* entity = _board[x][index.y];
		if (entity == nullptr) {
			break;
		}
		const auto gemsComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
		if (gemsComponent->gemType() != gemType) {
			break;
		}

		count.right++;
	}

	// Search up
	for (int y = index.y + 1; y < k_numGemsY; y++) {
		Entity* entity = _board[index.x][y];
		if (entity == nullptr) {
			break;
		}
		const auto gemsComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
		if (gemsComponent->gemType() != gemType) {
			break;
		}

		count.up++;
	}

	// Search down
	for (int y = index.y - 1; y >= 0; y--) {
		Entity* entity = _board[index.x][y];
		if (entity == nullptr) {
			break;
		}
		const auto gemsComponent = static_cast<GemsComponent*>(entity->getComponentWithType(ComponentType::Gem));
		if (gemsComponent->gemType() != gemType) {
			break;
		}

		count.down++;
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
		BackToBackCount count = backToBackCountOnIndex(index, gemType);
		if (count.up < 2 && count.down < 2 && count.left < 2 && count.right < 2) {
			return gemType;
		}
	}

	return GemType::Green;
}
