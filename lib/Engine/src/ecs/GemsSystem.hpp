#pragma once

#include <array>

#include <glm/vec2.hpp>

#include "ECS.hpp"
#include "GemsComponent.hpp"
#include <random>

class Entity;
class Engine;

enum class GameState {
    INVALID = -1,
    Initializing,
    Running,
    Ended
};

struct BackToBackCount {
	GemsComponent* gems[k_numMaxGemsComponents]{ nullptr };
	unsigned short numGems = 0;
};

struct NewBackToBackCount {
	GemsComponent* horizontalGems[k_numGemsX]{ nullptr };
	unsigned short numHorizontalGems = 0;
	GemsComponent* verticalGems[k_numGemsY]{ nullptr };
	unsigned short numVerticalGems = 0;
};


class GemsSystem {
public:
    bool initialize(Engine* engine);

    void update(float delta);

    Engine* engine() const { return _engine; }

	void moveEntityFromTo(const glm::ivec2& fromIndex, const glm::ivec2& toIndex);
	void swapGems(GemsComponent* firstComponent, GemsComponent* secondComponent);
	void cancelGemSwap();
	void removeEntity(const glm::ivec2 index);

	void onGemClicked(GemsComponent* gemComponent);

    GemsComponent* createComponent(RenderComponent* renderComponent);
    void releaseComponent(GemsComponent* component);

	void createNewRandomGemOnIndex(const glm::ivec2& index);
	BackToBackCount backToBackCountOnIndex(const glm::ivec2& index, const GemType gemType);
	NewBackToBackCount theNewBackToBackCount(const glm::ivec2& index, const GemType gemType);
	GemType randomPossibleGemForIndex(const glm::ivec2& index);

private:
	std::default_random_engine randomGenerator;
    std::array<GemsComponent, k_numMaxGemsComponents> _components;
    std::vector<Entity*> _waiting; // 8*8
	std::vector<Entity*> _dirty; // 8*8
	std::vector<Entity*> _swapedEntities; // 2
    Engine* _engine = nullptr;
    Entity* _board[8][8] = { nullptr };
	GemsComponent* _selectedGem = nullptr;
    GameState _gameState = GameState::INVALID;
    long _lastTimeSpawned[8] = {0};

	void updateGemPointers(GemsComponent* gemComponent);
};

