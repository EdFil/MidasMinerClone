#pragma once

#include "ECS.hpp"
#include <glm/vec2.hpp>

class TransformSystem;

class TransformComponent : public Component {
public:
	void release() override;
	void initialize();

	TransformSystem* _system;
    glm::vec2 position;

	friend TransformSystem;
};