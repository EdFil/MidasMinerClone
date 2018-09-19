#pragma once

#include "ECS.hpp"

#include <array>

#include "TransformComponent.hpp"

class TransformSystem : public System {
public:
    bool initialize() override;
    bool update(float delta) override;

    TransformComponent* createComponent();

private:
    std::array<TransformComponent, k_numMaxTransformComponents> _components;
};