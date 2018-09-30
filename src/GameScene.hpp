#pragma once

#include <memory>

#include <Scene.hpp>

class Entity;
class GameManager;

class GameScene : public Scene {
public:
	GameScene();
	~GameScene();

	void onCreated() override;
	void update(float delta) override;
	void onDestroy() override;

private:
	Entity* _background{nullptr};
    float _timeLeft{0.0f};
    std::unique_ptr<GameManager> _gameManager;

    // Auxiliary functions
    void createBackgroundEntity();
};