#pragma once

#include <array>
#include <random>
#include <memory>

#include <EventDispatcher.hpp>
#include <glm/vec2.hpp>

#include "GameConstants.hpp"
#include "GemsComponent.hpp"

class Entity;
class Engine;

struct NewBackToBackCount {
	GemsComponent* horizontalGems[k_numGemsX]{ nullptr };
	unsigned short numHorizontalGems = 0;
	GemsComponent* verticalGems[k_numGemsY]{ nullptr };
	unsigned short numVerticalGems = 0;
};

class GemsSystem : public KeyEventDelegate {
public:
    bool initialize(Engine* engine);
    void cleanup();
    void update(float delta);

    bool onKeyDown(const SDL_Keysym& keySym) override;
    bool onKeyUp(const SDL_Keysym& keySym) override;

	void startGame();
	void restartGame();
	void clearBoard();
	void setIsClickable(bool isClickable);

    Engine* engine() const { return _engine; }
	static glm::vec2 positionForIndex(const glm::ivec2& index);

	void makeGemFallFromTo(const glm::ivec2& fromIndex, const glm::ivec2& toIndex);
	bool trySwapGem(GemsComponent* gemComponent, const glm::ivec2& index);
	void swapGems(GemsComponent* firstComponent, GemsComponent* secondComponent);
	void cancelGemSwap(const std::pair<GemsComponent*, GemsComponent*>&  swapPair);
	bool tryChainDelete(GemsComponent* component);
	void removeEntity(const glm::ivec2 index);

	// Query the system functions
	bool areAllGemsResting() const;

	// Gameplay Callbacks
	void onGemClicked(GemsComponent* gemComponent);
	void onGemSwipedLeft(GemsComponent* gemComponent);
	void onGemSwipedRight(GemsComponent* gemComponent);
	void onGemSwipedUp(GemsComponent* gemComponent);
	void onGemSwipedDown(GemsComponent* gemComponent);
	void onGemsSwapped(GemsComponent* firstComponent, GemsComponent* secondComponent);
	void onFinishedFalling(GemsComponent* gemComponent);

    GemsComponent* createComponent(RenderComponent* renderComponent);
    void releaseComponent(GemsComponent* component);

	void createNewRandomGemOnIndex(const glm::ivec2& index);
	NewBackToBackCount theNewBackToBackCount(const glm::ivec2& index, const GemType gemType);
	GemType randomPossibleGemForIndex(const glm::ivec2& index);

private:
	std::default_random_engine randomGenerator;
    std::array<GemsComponent, k_numMaxGemsComponents> _components;
    std::vector<Entity*> _waiting;
	std::vector<Entity*> _gemsThatFell;
	std::vector<std::pair<GemsComponent*, GemsComponent*>> _swappedGems;
    Engine* _engine = nullptr;
    Entity* _board[k_numGemsX][k_numGemsY] = {nullptr};
	Entity* _selectedGemCross{nullptr};
	unsigned _currentFrameSpawnOffset[k_numGemsX] = {0};
	GemsComponent* _selectedGem = nullptr;
	bool _isClickable = false;
};

