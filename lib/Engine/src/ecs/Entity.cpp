#include "Entity.hpp"

#include <algorithm>

#include "EntitySystem.hpp"

void Entity::release() {
	_system->releaseEntity(this);
}

Component* Entity::getComponentWithType(ComponentType type) {
	for (Component* component : _components) {
		if (component->type == type) {
			return component;
		}
	}

	return nullptr;
}

void Entity::addComponent(Component* component) {
	_components.push_back(component);
}

void Entity::removeComponent(Component* component) {
	_components.erase(std::remove(_components.begin(), _components.end(), component), _components.end());
}

void Entity::cleanup() {
	for(Component* component : _components) {
		component->release();
	}
}
