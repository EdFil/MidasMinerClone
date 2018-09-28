#pragma once

#include <array>
#include <random>

#include <glm/vec2.hpp>

#include "GameConstants.hpp"
#include "GemsComponent.hpp"

class Entity;
class Engine;

enum class GameState {
    INVALID = -1,
    Initializing,
    Running,
    Ended
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
	static glm::vec2 positionForIndex(const glm::ivec2& index);

	void moveEntityFromTo(const glm::ivec2& fromIndex, const glm::ivec2& toIndex);
	bool trySwapGem(GemsComponent* gemComponent, const glm::ivec2& index);
	void swapGems(GemsComponent* firstComponent, GemsComponent* secondComponent);
	void cancelGemSwap(const std::pair<GemsComponent*, GemsComponent*>&  swapPair);
	bool tryChainDelete(GemsComponent* component);
	void removeEntity(const glm::ivec2 index);

	void onGemClicked(GemsComponent* gemComponent);
	void onGemSwipedLeft(GemsComponent* gemComponent);
	void onGemSwipedRight(GemsComponent* gemComponent);
	void onGemSwipedUp(GemsComponent* gemComponent);
	void onGemSwipedDown(GemsComponent* gemComponent);
	void onGemsSwapped(GemsComponent* firstComponent, GemsComponent* secondComponent);
	void onFinishedFalling(GemsComponent* gemComponent);

    GemsComponent* createComponent(RenderComponent* renderComponent);
    void releaseComponent(GemsComponent* component);

	void createNewRandomGemOnIndex(const glm::ivec2& index);
	NewBackToBackCount theNewBackToBackCount(const glm::ivec2& index, const GemType gemType);
	GemType randomPossibleGemForIndex(const glm::ivec2& index);

private:
	std::default_random_engine randomGenerator;
    std::array<GemsComponent, k_numMaxGemsComponents> _components;
    std::vector<Entity*> _waiting;
	std::vector<Entity*> _dirty;
	std::vector<std::pair<GemsComponent*, GemsComponent*>> _swapedGems;
    Engine* _engine = nullptr;
    Entity* _board[k_numGemsX][k_numGemsY] = {nullptr};
	Entity* _selectedGemCross;
	unsigned _currentFrameSpawnOffset[k_numGemsX] = {0};
	GemsComponent* _selectedGem = nullptr;
    GameState _gameState = GameState::INVALID;
};

