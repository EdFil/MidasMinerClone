#pragma once

#include "Entity.hpp"

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

private:
    GameState _gameState = GameState::INVALID;
    Engine* _engine = nullptr;
    std::vector<Entity*> _waiting; // 8*8
    Entity* _board[8][8] = { nullptr };
    long _lastTimeSpawned[8] = {0};
};

