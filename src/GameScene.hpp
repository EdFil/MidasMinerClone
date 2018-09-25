#pragma once

#include <memory>

#include <Scene.hpp>

class GemsSystem;

class GameScene : public Scene {
public:
	GameScene();

	void onCreated() override;
	void update(float delta) override;
	void onDestroy() override;

private:
	std::unique_ptr<GemsSystem> _gemsSystem;
};