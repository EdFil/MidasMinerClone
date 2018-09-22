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

	void onAddedToBoard(int x, int y);
	void onMovedInBoard(int toX, int toY);
	void onRemovedFromBoard();

	GemType gemType() const { return _gemType; }
	void setGemType(GemType gemType);
    void setIsActive(bool value);

    void onLeftMouseDown(int x, int y) override;
    void onLeftMouseUp(int x, int y) override;
    void onMouseMotion(int x, int y) override;

private:
	glm::vec<2, int> _boardIndex{-1, -1};
    GemsSystem* _system = nullptr;
    RenderComponent* _renderComponent = nullptr;
	GemType _gemType = GemType::INVALID;
    bool _isActive = false;
};
