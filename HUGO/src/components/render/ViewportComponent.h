#pragma once
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
    // Setters
    inline void setLeft(float l) { left = l; }
    inline void setTop(float t) { top = t; }
    inline void setWidth(float w) { width = w; }
    inline void setHeight(float h) { height = h; }
    inline void setZOrder(int z) { zOrder = z; }
    inline void setClearColor(const HUGO::Vector4& color) { clearColor = color; }
    inline void setClearDepth(bool clear) { clearDepth = clear; }
    inline void setClearStencil(bool clear) { clearStencil = clear; }
    inline void setCameraEntity(EntityID camera) { cameraEntity = camera; }
    inline void setActive(bool act) { active = act; }
    // Getters
    inline float getLeft() const { return left; }
    inline float getTop() const { return top; }
    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }
    inline int getZOrder() const { return zOrder; }
    inline HUGO::Vector4 getClearColor() const { return clearColor; }
    inline bool getClearDepth() const { return clearDepth; }
    inline bool getClearStencil() const { return clearStencil; }
    inline EntityID getCameraEntity() const { return cameraEntity; }
    inline bool isActive() const { return active; }

END_HCOMPONENT()