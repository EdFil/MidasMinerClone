#include "GameScene.hpp"

#include <SDL_render.h>
#include <SDL_log.h>

#include <Engine.hpp>
#include <TextureManager.hpp>
#include <ecs/EntitySystem.hpp>
#include <ecs/TransformSystem.hpp>
#include <ecs/RenderSystem.hpp>

#include "GemsSystem.hpp"

GameScene::GameScene() { /* Needs to be defined on the cpp because of forward declaration of GemsSystem */ }

void GameScene::onCreated() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onCreated");

	// Gems system
	_gemsSystem = std::make_unique<GemsSystem>();
	_gemsSystem->initialize(_engine);

	// Background Entity
	TransformComponent* transform = _engine->transformSystem()->createComponent();

	SDL_Texture* backgroundTexture = _engine->textureManager()->loadTexture(TextureID::Background);
	RenderComponent* renderComponent = _engine->renderSystem()->createComponent(transform, backgroundTexture);

	_background = _engine->entitySystem()->createEntity();
	_background->addComponent(transform);
	_background->addComponent(renderComponent);

	_gemsSystem->createScoreLabels();
}

void GameScene::update(float delta) {
	_gemsSystem->update(delta);
}

void GameScene::onDestroy() {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameScene::onDestroy");
	_engine->entitySystem()->releaseEntity(_background);
}
