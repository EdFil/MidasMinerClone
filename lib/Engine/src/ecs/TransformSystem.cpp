#include "TransformSystem.hpp"

bool TransformSystem::initialize() {
	for(TransformComponent& component : _components) {
		component._system = this;
		component.type = ComponentType::Transform;
	}

	return true;
}

TransformComponent* TransformSystem::createComponent() {
    for(TransformComponent& component : _components) {
        if(!component.isUsed) {
            component.initialize();
			component.isUsed = true;
            return &component;
        }
    }

    return nullptr;
}

void TransformSystem::releaseComponent(TransformComponent* component) {
	component->isUsed = false;
}
