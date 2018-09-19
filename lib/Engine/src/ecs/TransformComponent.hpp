#pragma once

#include "ECS.hpp"

#include <glm/vec2.hpp>

struct TransformComponent : public Component {
public:
    static constexpr ComponentType TYPE = ComponentType::Transform;

    void initialize() {
        position = {0.0f, 0.0f};
        state = State::Used;
    }

    glm::vec2 position;
};
