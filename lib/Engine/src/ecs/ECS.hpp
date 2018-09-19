#pragma once

#include <array>
#include <vector>

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
    State state = State::Unused;
};

class System {
    virtual bool initialize() = 0;
    virtual bool update(float delta) = 0;
};

struct Entity {
    State state = State::Unused;

    void addComponent(Component* component) { _components.push_back(component); }
    void removeComponent(Component* component) { _components.push_back(component); }

    std::vector<Component*> _components;
};