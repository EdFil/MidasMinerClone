#pragma once

class Entity;

enum class ComponentType {
	Transform = 0,
	Render = 1,
	Text,
	Gem,
	COUNT
};

struct Component {
	ComponentType type;
	bool isUsed = false;

	virtual ~Component() = default;
	
	virtual void release() = 0;

	Entity* entity() const { return _entity; }

protected:
	Entity* _entity = nullptr;

	void setEntity(Entity* entity) { _entity = entity; }

	friend Entity;
};
