#include "MainMenuScene.hpp"

#include <SDL_log.h>

#include <Engine.hpp>

#include "GameScene.hpp"

void MainMenuScene::onCreated() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "MainMenuScene ::onCreated");
}

void MainMenuScene::update(float) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "MainMenuScene ::onUpdate");
	_engine->setScene(std::make_unique<GameScene>());
}

void MainMenuScene::onDestroy() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "MainMenuScene ::onDestroy");
}
