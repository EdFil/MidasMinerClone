#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_int2.hpp>
#include <ecs/Component.hpp>
#include <EventDispatcher.hpp>

class GemsSystem;
class RenderComponent;

enum class GemType {
	INVALID = -1,
	Blue = 0,
	Green,
	Purple,
	Red,
	Yellow,
	COUNT
};

enum class GemStatus {
	INVALID = -1,
	Despawned,
	Spawned,
	Rest,
	Falling,
	Swapping,
	Swapped,
	SwappingBack,
	BeeingDestroyed
};

class GemsComponent : public Component, public MouseEventDelegate {
public:
	void release() override;

    bool initialize(GemsSystem* system, RenderComponent* renderComponent);
    void cleanup();

	void onAddedToBoard(const glm::ivec2& index);
	void onSwap(GemsComponent& gemComponent);
	void onSwapBackToIndex(const glm::ivec2& index);
	void onMovedInBoard(const glm::vec<2, int>& index);
	void onRemovedFromBoard();
	void update(float delta);

	const glm::ivec2& index() const { return _boardIndex; }
	GemType gemType() const { return _gemType; }
	GemStatus gemStatus() const { return _gemStatus; }
	bool isActive() const { return _gemStatus != GemStatus::INVALID && _gemStatus != GemStatus::Despawned; }
	bool canSwap() const { return _gemStatus == GemStatus::Rest; }
	bool canBeRemoved() const { return _gemStatus == GemStatus::Rest || _gemStatus == GemStatus::Swapped; }
	bool canBeMatchedWith(const GemType gemType) const;

	void setGemType(GemType gemType);

	bool onLeftMouseDown(int x, int y) override;
	bool onLeftMouseUp(int x, int y) override;
	bool onMouseMotion(int x, int y) override;

private:
	glm::vec2 _finalPosition;
	glm::ivec2 _boardIndex{-1, -1};
    GemsSystem* _system = nullptr;
	GemsComponent* _swappingWith = nullptr;
    RenderComponent* _renderComponent = nullptr;
	GemType _gemType = GemType::INVALID;
	GemStatus _gemStatus = GemStatus::INVALID;
	glm::ivec2 _mouseDownPosition{-1};
	bool _isDragging = false;

    friend GemsSystem;
};
