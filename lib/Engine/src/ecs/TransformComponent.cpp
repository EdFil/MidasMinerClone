#include "TransformComponent.hpp"

#include "TransformSystem.hpp"

void TransformComponent::initialize() {
    _position = {0.0f, 0.0f};
}

void TransformComponent::release() {
	_system->releaseComponent(this);
}
