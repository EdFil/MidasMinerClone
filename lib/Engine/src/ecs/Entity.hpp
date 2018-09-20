#pragma once

#include "ECS.hpp"

#include <vector>

class EntitySystem;

class Entity {
public:
	void release();

	Component* getComponentWithType(ComponentType type);
	void addComponent(Component* component);
	void removeComponent(Component* component);

	State state = State::Unused;
	EntitySystem* _system;
	std::vector<Component*> _components;

private:
	void cleanup();

	friend EntitySystem;
};