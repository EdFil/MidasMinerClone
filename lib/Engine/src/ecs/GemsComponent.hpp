#pragma once

#include "ECS.hpp"

#include <glm/vec2.hpp>

#include "EventDispatcher.hpp"

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

class GemsComponent : public Component, public MouseEventDelegate {
public:
	void release() override;

    bool initialize(GemsSystem* system, RenderComponent* renderComponent);
    void cleanup();

	void onAddedToBoard(const glm::vec<2, int>& index);
	void onMovedInBoard(const glm::vec<2, int>& index);
	void onRemovedFromBoard();

	const glm::vec<2, int>& index() const { return _boardIndex; }
	GemType gemType() const { return _gemType; }
	GemsComponent* upGem() const { return _up; }
	GemsComponent* downGem() const { return _down; }
	GemsComponent* leftGem() const { return _left; }
	GemsComponent* rightGem() const { return _right; }

	void setGemType(GemType gemType);

	bool onLeftMouseDown(int x, int y) override;
	bool onLeftMouseUp(int x, int y) override;
	bool onMouseMotion(int x, int y) override;

private:
	glm::vec<2, int> _boardIndex{-1, -1};
	GemsComponent* _up = nullptr;
	GemsComponent* _down = nullptr;
	GemsComponent* _left = nullptr;
	GemsComponent* _right = nullptr;
    GemsSystem* _system = nullptr;
    RenderComponent* _renderComponent = nullptr;
	GemType _gemType = GemType::INVALID;
    bool _isActive = false;

    friend GemsSystem;
};
