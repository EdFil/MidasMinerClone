#pragma once

#include "ECS.hpp"
#include <glm/vec2.hpp>

class TransformSystem;

class TransformComponent : public Component {
public:
	void release() override;
	void initialize();

	const glm::vec2& position() const { return _position; }
	void setPosition(const glm::vec2& position) { _position = position; }
	void setPosition(float x, float y) { setPosition(glm::vec2(x, y)); }

private:
	TransformSystem* _system;
    glm::vec2 _position;

	friend TransformSystem;
};