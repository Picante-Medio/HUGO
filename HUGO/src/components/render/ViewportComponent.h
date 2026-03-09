#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoComponentMacros.h"

HCOMPONENT(ViewportComponent)

private:

    // Coordinates reletad to the window (0-1)
    float left = 0.0f;
    float top = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
    
    // Render order (lower renders first)
    int zOrder = 0;
    
    // Background color and clear options
    HUGO::Vector4 clearColor = HUGO::Vector4(0.3f, 0.3f, 0.5f, 1.0f);
    bool clearDepth = true;
    bool clearStencil = true;
    
    // Camera associated with this viewport
    EntityID cameraEntity = 0;
    
    bool active = true;

public:

    ViewportComponent(EntityID entity, float l, float t = 0.0f, float w = 1.0f, float h = 1.0f, int z = 0)
        : Component(entity), left(l), top(t), width(w), height(h), zOrder(z) {}

    // Setters
    void setLeft(float l) { left = l; }
    void setTop(float t) { top = t; }
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }
    void setZOrder(int z) { zOrder = z; }
    void setClearColor(const HUGO::Vector4& color) { clearColor = color; }
    void setClearDepth(bool clear) { clearDepth = clear; }
    void setClearStencil(bool clear) { clearStencil = clear; }
    void setCameraEntity(EntityID camera) { cameraEntity = camera; }
    void setActive(bool active) { this->active = active; }
    // Getters
    float getLeft() const { return left; }
    float getTop() const { return top; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    int getZOrder() const { return zOrder; }
    HUGO::Vector4 getClearColor() const { return clearColor; }
    bool getClearDepth() const { return clearDepth; }
    bool getClearStencil() const { return clearStencil; }
    EntityID getCameraEntity() const { return cameraEntity; }
    bool isActive() const { return active; }

END_HCOMPONENT()