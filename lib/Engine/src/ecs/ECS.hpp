#pragma once

constexpr int k_numMaxEntities = 100;
constexpr int k_numMaxTransformComponents = k_numMaxEntities;
constexpr int k_numMaxRenderComponents = k_numMaxEntities;

enum class ComponentType {
    Transform = 0,
    Render = 1,
    Clickable,
    COUNT
};

enum class State { Unused, Used };

struct Component {
	ComponentType type;
    State state = State::Unused;

	virtual void release() = 0;
};

class System {
    virtual bool initialize() = 0;
    virtual bool update(float delta) = 0;
};