#pragma once

#include <Scene.hpp>

class MainMenuScene : public Scene {

	void onCreated() override;
	void update(float delta) override;
	void onDestroy() override;
};