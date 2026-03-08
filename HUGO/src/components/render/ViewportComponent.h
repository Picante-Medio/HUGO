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

    ViewportComponent(EntityID entity,
                    float l, float t = 0.0f, float w = 1.0f, float h = 1.0f, int z = 0);

    // Setters
    void setLeft(float l);
    void setTop(float t);
    void setWidth(float w);
    void setHeight(float h);
    void setZOrder(int z);
    void setClearColor(const HUGO::Vector4& color);
    void setClearDepth(bool clear);
    void setClearStencil(bool clear);
    void setCameraEntity(EntityID camera);
    void setActive(bool active);

    // Getters
    float getLeft() const;
    float getTop() const;
    float getWidth() const;
    float getHeight() const;
    int getZOrder() const;
    HUGO::Vector4 getClearColor() const;
    bool getClearDepth() const;
    bool getClearStencil() const;
    EntityID getCameraEntity() const;   
    bool isActive() const;

END_HCOMPONENT()