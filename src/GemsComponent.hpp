#pragma once

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

	void setGemType(GemType gemType);

	bool onLeftMouseDown(int x, int y) override;
	bool onLeftMouseUp(int x, int y) override;
	bool onMouseMotion(int x, int y) override;

private:
	glm::vec<2, int> _boardIndex{-1, -1};
    GemsSystem* _system = nullptr;
    RenderComponent* _renderComponent = nullptr;
	GemType _gemType = GemType::INVALID;
    bool _isActive = false;

    friend GemsSystem;
};
