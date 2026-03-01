#pragma once

#include "Component.h"
#include "HugoMath.h"

struct ViewportComponent: public Component
{
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
    
    ViewportComponent(EntityID entity,
                    float l = 0.0f, float t = 0.0f, float w = 1.0f, float h = 1.0f, int z = 0)
      : Component(entity)
      , left(l), top(t), width(w), height(h), zOrder(z) {}
};