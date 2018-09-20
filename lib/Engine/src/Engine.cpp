#include "Engine.hpp"

#include <cstdlib>

#include <SDL.h>
#include <SDL_log.h>

#include "TextureManager.hpp"
#include "ecs/ECS.hpp"

SDL_Texture* _background;
Entity* _entity;

Engine::Engine() { }
Engine::~Engine() { }

bool Engine::initialize() {
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_Init Error: %s", SDL_GetError());
		return false;
	}

	_window = SDL_CreateWindow("ConceptV2", 100, 100, 755, 600, SDL_WINDOW_SHOWN);
	if (_window == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateWindow Error: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (_renderer == nullptr) {
		SDL_DestroyWindow(_window);
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "SDL_CreateRenderer Error: %s", SDL_GetError());
		SDL_Quit();
		return false;
	}

	_textureManager = std::make_unique<TextureManager>(this);
    _eventDispatcher = std::make_unique<EventDispatcher>();

	_transformSystem.initialize();
	_renderSystem.initialize();
    _eventDispatcher->initialize();

    _background = _textureManager->loadTexture(TextureID::Background);
    _eventDispatcher->registerForApplicationEvents(this);

	return true;
}

void Engine::cleanup() {
    _eventDispatcher->unregisterForApplicationEvents(this);

	if(_renderer)
		SDL_DestroyRenderer(_renderer);
	if(_window)
		SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Engine::run() {
	_isRunning = true;
	mainLoop();
}

void Engine::mainLoop() {
	while (_isRunning) {
		_eventDispatcher->update();

        static int numFrames = 0;
        if(numFrames++ % 200 < 100 && _entity == nullptr) {
			TransformComponent* transformComponent = _transformSystem.createComponent();
			RenderComponent* renderComponent = _renderSystem.createComponent(transformComponent, _textureManager->loadTexture(TextureID:: Red));

			_entity = _entitySystem.createEntity();
			_entity->addComponent(transformComponent);
			_entity->addComponent(renderComponent);
        } else if (numFrames % 200 >= 100 && _entity != nullptr) {
            _entity->release();
            _entity = nullptr;
        }

		// Render Scene
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		SDL_RenderClear(_renderer);
	
		SDL_RenderCopy(_renderer, _background, nullptr, nullptr);

        _renderSystem.draw(_renderer);

		SDL_RenderPresent(_renderer);
	}
}

void Engine::onQuit() {
    _isRunning = false;
}
