#pragma once

#include "ECS.hpp"

#include <array>

#include "TransformComponent.hpp"

class TransformSystem {
public:
    bool initialize();

    TransformComponent* createComponent();
	void releaseComponent(TransformComponent* component);
private:
    std::array<TransformComponent, k_numMaxTransformComponents> _components;
};