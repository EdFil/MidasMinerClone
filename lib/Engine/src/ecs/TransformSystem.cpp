#include "TransformSystem.hpp"

bool TransformSystem::initialize() { return true; }

bool TransformSystem::update(float) { return true; }

TransformComponent* TransformSystem::createComponent() {
    for(TransformComponent& component : _components) {
        if(component.state == State::Unused) {
            component.initialize();
            return &component;
        }
    }

    return nullptr;
}