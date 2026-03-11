#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

// Camera component that holds all necessary information for rendering a camera in the scene
HCOMPONENT(CameraComponent)

private:
    HUGO::ProjectionType projectionType = HUGO::ProjectionType::Perspective;
    
    // Perspective parameters
    float fovY = 60.0f; // Field of view in the Y direction, in degrees
    float nearClip = 1.0f;
    float farClip = 10000.0f;

    // Orthographic parameters
    float orthoWidth = 10.f;
    float orthoHeight = 10.f;

    //Aspect ratio
    float aspectRatio = 16.0f / 9.0f;
    bool autoAspectRatio = true;

    bool active = true;
    int priority = 0; // for multiple cameras, lower priority renders first
    
    // Mode
    HUGO::PolygonMode polygonMode = HUGO::PolygonMode::SOLID;
  
public:
    // Setters
    void setProjectionType(HUGO::ProjectionType type) { projectionType = type; }
    void setFOVY(float fov) { fovY = fov; }
    void setNearClip(float nearClip) { this->nearClip = nearClip; }
    void setFarClip(float farClip) { this->farClip = farClip; }
    void setOrthographicDimensions(float width, float height) { orthoWidth = width; orthoHeight = height; }
    void setAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }
    void setAutoAspectRatio(bool autoAspectRatio) { this->autoAspectRatio = autoAspectRatio; }
    void setActive(bool active) { this->active = active; }
    void setPriority(int priority) { this->priority = priority; }
    void setPolygonMode(HUGO::PolygonMode mode) { polygonMode = mode; }
    // Getters
    HUGO::ProjectionType getProjectionType() const { return projectionType; }
    float getFOVY() const { return fovY; }
    float getNearClip() const { return nearClip; }
    float getFarClip() const { return farClip; }
    std::pair<float, float> getOrthographicDimensions() const { return {orthoWidth, orthoHeight}; }
    float getAspectRatio() const { return aspectRatio; }
    bool getAutoAspectRatio() const { return autoAspectRatio; }
    bool isActive() const { return active; }
    int getPriority() const { return priority; }
    HUGO::PolygonMode getPolygonMode() const { return polygonMode; }

END_HCOMPONENT()