#pragma once

#include "Entity.hpp"

enum class GameState {
    Initializing,
    Running,
    Ended
};

class GemsSystem {
public:
    bool initialize();

    void update(float delta);

private:
    std::vector<Entity*> _waiting; // 8*8
    Entity* _board[8][8];
};

