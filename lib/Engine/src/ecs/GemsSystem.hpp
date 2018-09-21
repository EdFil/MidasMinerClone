#pragma once

#include <array>

#include "ECS.hpp"
#include "GemsComponent.hpp"

class Entity;
class Engine;

enum class GameState {
    INVALID = -1,
    Initializing,
    Running,
    Ended
};

class GemsSystem {
public:
    bool initialize(Engine* engine);

    void update(float delta);

    Engine* engine() const { return _engine; }

    GemsComponent* createComponent(RenderComponent* renderComponent);
    void releaseComponent(GemsComponent* component);

private:
    std::array<GemsComponent, k_numMaxGemsComponents> _components;
    std::vector<Entity*> _waiting; // 8*8
    Engine* _engine = nullptr;
    Entity* _board[8][8] = { nullptr };
    GameState _gameState = GameState::INVALID;
    long _lastTimeSpawned[8] = {0};
};

