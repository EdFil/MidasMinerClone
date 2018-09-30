#include "GameManager.hpp"

#include <SDL_assert.h>
#include <Engine.hpp>
#include <EventDispatcher.hpp>
#include <TextureManager.hpp>

#include "GemsSystem.hpp"

GameManager::GameManager(Engine* engine) : _engine(engine) { }
GameManager::~GameManager() { /* Needs to be defined on the cpp because of forward declaration of GemsSystem */ }

void GameManager::initialize() {
    SDL_assert(_engine != nullptr);

    _engine->eventDispatcher()->registerForKeyEvents(this);

    _gemsSystem = std::make_unique<GemsSystem>();
    _gemsSystem->initialize(_engine);
}

void GameManager::cleanup() {
    _engine->eventDispatcher()->unregisterForKeyEvents(this);
    _gemsSystem->cleanup();
}

void GameManager::update(float delta) {
    // Check game state
    if(_gameState == GameState::Running) {
        _timeLeft -= delta;
        updateTimeLabel();

        if (_timeLeft <= 0) {
            _gameState = GameState::Ended;
            stopGame();
            return;
        }

        _gemsSystem->update(delta);

    } else if (_gameState == GameState::Initializing) {
        _gemsSystem->update(delta);

        if(_gemsSystem->areAllGemsResting()) {
            _gameState = GameState::Running;
            _gemsSystem->setIsClickable(true);
        }
    } else if(_gameState == GameState::Ended) {
        return;
    }
}


void GameManager::createForegroundLabels() {
    createTimeCounter();
    createGameOverScreen();
}

void GameManager::startGame() {
    _timeLeft = 60.0f;
    updateTimeLabel();
    setTimeLabelVisible(true);

    setGameOverScreenVisible(false);
    _gameState = GameState::Initializing;
}

void GameManager::stopGame() {
    _gemsSystem->clearBoard();
    _gemsSystem->setIsClickable(false);
    setGameOverScreenVisible(true);
}


bool GameManager::onKeyDown(const SDL_Keysym &keySym) {
    if(_gameState == GameState::Ended) {
        startGame();
    }

    return true;
}

bool GameManager::onKeyUp(const SDL_Keysym&) {
    // Do nothing here. just consume the key press
    return true;
}

void GameManager::createTimeCounter() {
    // Time label
    TransformComponent* transform = _engine->transformSystem()->createComponent();
    transform->setPosition({90, 430});
    RenderComponent* render = _engine->renderSystem()->createComponent(transform, nullptr);
    TextComponent* text = _engine->textSystem()->createComponent(render);
    _timeEntity = _engine->entitySystem()->createEntity();
    _timeEntity->addComponent(transform);
    _timeEntity->addComponent(render);
    _timeEntity->addComponent(text);
}

void GameManager::updateTimeLabel() {
    SDL_assert(_timeEntity != nullptr);
    SDL_Color textColor = (_timeLeft < 10.0f) ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 0, 0, 255, 255};
    TextComponent *textComponent = static_cast<TextComponent *>(_timeEntity->getComponentWithType(ComponentType::Text));
    textComponent->setText(std::to_string(static_cast<int>(std::ceil(_timeLeft))), TextureManager::k_defaultFontName, 50, textColor);
}

void GameManager::setTimeLabelVisible(bool isVisible) {
    SDL_assert(_timeEntity != nullptr);
    auto render = static_cast<RenderComponent*>(_timeEntity->getComponentWithType(ComponentType::Render));
    render->setIsVisible(isVisible);
}

void GameManager::createGameOverScreen() {
    // GameOver label
    TransformComponent* transform = _engine->transformSystem()->createComponent();
    transform->setPosition({335.0f, 220.0f});
    RenderComponent* render = _engine->renderSystem()->createComponent(transform, nullptr);
    render->setIsVisible(false);
    TextComponent* text = _engine->textSystem()->createComponent(render);
    text->setText("GAME OVER", TextureManager::k_defaultFontName, 55, SDL_Color{ 255, 0, 0});
    _gameOver = _engine->entitySystem()->createEntity();
    _gameOver->addComponent(transform);
    _gameOver->addComponent(render);
    _gameOver->addComponent(text);

    // Press any key label
    transform = _engine->transformSystem()->createComponent();
    transform->setPosition({345.0f, 300.0f});
    render = _engine->renderSystem()->createComponent(transform, nullptr);
    render->setIsVisible(false);
    text = _engine->textSystem()->createComponent(render);
    text->setText("Press any key to restart", TextureManager::k_defaultFontName, 25, SDL_Color{ 255, 0, 0});
    _pressAnyButtonEntity = _engine->entitySystem()->createEntity();
    _pressAnyButtonEntity->addComponent(transform);
    _pressAnyButtonEntity->addComponent(render);
    _pressAnyButtonEntity->addComponent(text);
}

void GameManager::setGameOverScreenVisible(bool isVisible) {
    SDL_assert(_gameOver != nullptr);
    SDL_assert(_pressAnyButtonEntity != nullptr);

    auto render = static_cast<RenderComponent*>(_gameOver->getComponentWithType(ComponentType::Render));
    render->setIsVisible(isVisible);

    render = static_cast<RenderComponent*>(_pressAnyButtonEntity->getComponentWithType(ComponentType::Render));
    render->setIsVisible(isVisible);
}
