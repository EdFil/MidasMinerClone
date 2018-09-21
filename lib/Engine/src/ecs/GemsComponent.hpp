#pragma once

#include "ECS.hpp"
#include "EventDispatcher.hpp"

class GemsSystem;
class RenderComponent;

class GemsComponent : public Component, public MouseEventDelegate {
public:
    void release() override;

    bool initialize(GemsSystem* system, RenderComponent* renderComponent);
    void cleanup();

    void setIsActive(bool value);

    void onLeftMouseDown(int x, int y);
    void onLeftMouseUp(int x, int y);
    void onMouseMotion(int x, int y);

private:
    GemsSystem* _system = nullptr;
    RenderComponent* _renderComponent = nullptr;
    bool _isActive = false;
};
