#pragma once

#include <memory>

#include "EventDispatcher.hpp"

class Entity;
class Engine;
class GemsSystem;

enum class GameState {
    INVALID = -1,
    Initializing,
    Running,
    Ended
};

class GameManager : public KeyEventDelegate {
public:
    GameManager(Engine* engine);
    ~GameManager();

    void initialize();
    void cleanup();
    void update(float delta);

    void createForegroundLabels();

    void startGame();
    void stopGame();

    bool onKeyDown(const SDL_Keysym& keySym) override;
    bool onKeyUp(const SDL_Keysym& keySym) override;

private:
    Engine* _engine{nullptr};
    Entity* _timeEntity{nullptr};
    Entity* _gameOver{nullptr};
    Entity* _pressAnyButtonEntity{nullptr};
    float _timeLeft{0.0f};
    std::unique_ptr<GemsSystem> _gemsSystem;
    GameState _gameState{GameState::INVALID};

    // Auxiliary functions
    void createTimeCounter();
    void updateTimeLabel();
    void setTimeLabelVisible(bool isVisible);
    void createGameOverScreen();
    void setGameOverScreenVisible(bool isVisible);
};