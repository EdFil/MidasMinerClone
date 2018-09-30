#include "Engine.hpp"

#include <cstdlib>

#include <SDL.h>
#include <SDL_log.h>

#include "TextureManager.hpp"
#include "Scene.hpp"
#include <ctime>

Engine::Engine() { }
Engine::~Engine() { }

bool Engine::initialize(std::unique_ptr<Scene>&& scene) {
	srand(time(nullptr));

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

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
	_textSystem.initialize(this);

	_textureManager->preloadAllTextures();
    _eventDispatcher->registerForApplicationEvents(this);
	setScene(std::move(scene));

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
	_lastGetTicksTime = SDL_GetTicks();
	mainLoop();
}

void Engine::setScene(std::unique_ptr<Scene>&& scene) {
	if(_runningScene != nullptr) {
		_runningScene->onDestroy();
	}

	_runningScene = std::move(scene);

	if(_runningScene != nullptr) {
		_runningScene->attachEngine(this);
		_runningScene->onCreated();
	}
}

void Engine::mainLoop() {

	while (_isRunning) {
		unsigned currentTime = SDL_GetTicks();
		float delta = static_cast<float>(currentTime - _lastGetTicksTime) / 1000.0f;
		_lastGetTicksTime = currentTime;

		_eventDispatcher->update();

		if(_runningScene != nullptr) {
			_runningScene->update(delta);
		}

		// Render Scene
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		SDL_RenderClear(_renderer);

        _renderSystem.draw(_renderer);

		SDL_RenderPresent(_renderer);
	}
}

void Engine::onQuit() {
    _isRunning = false;
}
