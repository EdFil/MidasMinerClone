#pragma once

#include <memory>

#include <Scene.hpp>

class Entity;
class GemsSystem;

class GameScene : public Scene {
public:
	GameScene();
	~GameScene();

	void onCreated() override;
	void update(float delta) override;
	void onDestroy() override;

private:
	Entity* _background{nullptr};
	std::unique_ptr<GemsSystem> _gemsSystem;
};