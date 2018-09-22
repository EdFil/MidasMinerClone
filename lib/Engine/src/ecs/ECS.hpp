#pragma once

#include <glm/vec2.hpp>

static const int k_numGemsX = 8;
static const int k_numGemsY = 8;
constexpr int k_numMaxEntities = 100;
constexpr int k_numMaxTransformComponents = k_numMaxEntities;
constexpr int k_numMaxRenderComponents = k_numMaxEntities;
constexpr int k_numMaxGemsComponents = k_numMaxEntities;

class Entity;

enum class ComponentType {
    Transform = 0,
    Render = 1,
    Gem,
    COUNT
};

enum class State { Unused, Used };

struct Component {
	Entity* _entity = nullptr;
	ComponentType type;
    State state = State::Unused;

	virtual void release() = 0;
};

class System {
    virtual bool initialize() = 0;
    virtual bool update(float delta) = 0;
};