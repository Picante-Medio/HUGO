#include "ViewportComponent.h"

ViewportComponent::ViewportComponent(EntityID entity, float l, float t, float w, float h, int z)
    : Component(entity), left(l), top(t), width(w), height(h), zOrder(z) {}


void ViewportComponent::setLeft(float l)
{
    left = l;
}

void ViewportComponent::setTop(float t)
{
    top = t;
}

void ViewportComponent::setWidth(float w)
{
    width = w;
}

void ViewportComponent::setHeight(float h)
{
    height = h;
}

void ViewportComponent::setZOrder(int z)
{
    zOrder = z;
}

void ViewportComponent::setClearColor(const HUGO::Vector4& color)
{
    clearColor = color;
}

void ViewportComponent::setClearDepth(bool clear)
{
    clearDepth = clear;
}

void ViewportComponent::setClearStencil(bool clear)
{
    clearStencil = clear;
}

void ViewportComponent::setCameraEntity(EntityID camera)
{
    cameraEntity = camera;
}

void ViewportComponent::setActive(bool active)
{
    this->active = active;
}

float ViewportComponent::getLeft() const
{
    return left;
}

float ViewportComponent::getTop() const
{
    return top;
}

float ViewportComponent::getWidth() const
{
    return width;
}

float ViewportComponent::getHeight() const
{
    return height;
}

int ViewportComponent::getZOrder() const
{
    return zOrder;
}

HUGO::Vector4 ViewportComponent::getClearColor() const
{
    return clearColor;
}

bool ViewportComponent::getClearDepth() const
{
    return clearDepth;
}

bool ViewportComponent::getClearStencil() const
{
    return clearStencil;
}

EntityID ViewportComponent::getCameraEntity() const
{
    return cameraEntity;
}

bool ViewportComponent::isActive() const
{
    return active;
}

