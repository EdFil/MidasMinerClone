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
	short up, down, left, right;
};

class GemsSystem {
public:
    bool initialize(Engine* engine);

    void update(float delta);

    Engine* engine() const { return _engine; }

	void moveEntityFromTo(int x1, int y1, int x2, int y2);
	void removeEntity(int x, int y);

    GemsComponent* createComponent(RenderComponent* renderComponent);
    void releaseComponent(GemsComponent* component);

	BackToBackCount backToBackCountOnIndex(const glm::vec<2, int> index, const GemType gemType);
	GemType randomPossibleGemForIndex(const glm::vec<2, int>& index);

private:
	std::default_random_engine randomGenerator;
    std::array<GemsComponent, k_numMaxGemsComponents> _components;
    std::vector<Entity*> _waiting; // 8*8
    Engine* _engine = nullptr;
    Entity* _board[8][8] = { nullptr };
    GameState _gameState = GameState::INVALID;
    long _lastTimeSpawned[8] = {0};
};

