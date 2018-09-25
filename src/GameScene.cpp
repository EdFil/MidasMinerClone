#include "GameScene.hpp"

#include <SDL_log.h>

#include <Engine.hpp>

#include "GemsSystem.hpp"

GameScene::GameScene() { }


void GameScene::onCreated() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onCreated");

	_gemsSystem = std::make_unique<GemsSystem>();
	_gemsSystem->initialize(_engine);
}

void GameScene::update(float delta) {
	_gemsSystem->update(delta);
}

void GameScene::onDestroy() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onDestroy");
}
