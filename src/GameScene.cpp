#include "GameScene.hpp"

#include <SDL_render.h>
#include <SDL_log.h>

#include <Engine.hpp>
#include <TextureManager.hpp>
#include <ecs/EntitySystem.hpp>
#include <ecs/TransformSystem.hpp>
#include <ecs/RenderSystem.hpp>

#include "GameManager.hpp"

GameScene::GameScene() { /* Needs to be defined on the cpp because of forward declaration of GemsSystem */ }
GameScene::~GameScene() { /* Needs to be defined on the cpp because of forward declaration of GemsSystem */ }

void GameScene::onCreated() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onCreated");

    _gameManager = std::make_unique<GameManager>(_engine);
    _gameManager->initialize();

	createBackgroundEntity();

    _gameManager->createForegroundLabels();
    _gameManager->startGame();
}

void GameScene::update(float delta) {
    _gameManager->update(delta);
}

void GameScene::onDestroy() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onDestroy");
	_gameManager->cleanup();
	_engine->entitySystem()->releaseEntity(_background);
}

void GameScene::createBackgroundEntity() {
	TransformComponent* transform = _engine->transformSystem()->createComponent();

	SDL_Texture* backgroundTexture = _engine->textureManager()->loadTexture(TextureID::Background);
	RenderComponent* renderComponent = _engine->renderSystem()->createComponent(transform, backgroundTexture);

	_background = _engine->entitySystem()->createEntity();
	_background->addComponent(transform);
	_background->addComponent(renderComponent);
}
