#pragma once

#include <vector>

enum class ComponentType;
enum class State;
class Component;
class EntitySystem;

class Entity {
public:
	void release();

	Component* getComponentWithType(ComponentType type);
	void addComponent(Component* component);
	void removeComponent(Component* component);

	EntitySystem* _system;
	std::vector<Component*> _components;
	bool _isUsed = false;

private:
	void cleanup();

	friend EntitySystem;
};