#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Component.hpp"

class TransformSystem;

class TransformComponent : public Component {
public:
	void initialize();
	void release() override;

	const glm::vec2& position() const { return _position; }
	void setPositionY(const float y) { _position.y = y; }
	void setPosition(const glm::vec2& position) { _position = position; }
	void setPosition(const float x, const float y) { setPosition(glm::vec2(x, y)); }

private:
	TransformSystem* _system{nullptr};
    glm::vec2 _position{0.0f, 0.0f};

	friend TransformSystem;
};