#pragma once
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
    inline void setProjectionType(HUGO::ProjectionType type) { projectionType = type; }
    inline void setFOVY(float fov) { fovY = fov; }
    inline void setNearClip(float nearC) { nearClip = nearC; }
    inline void setFarClip(float farClip) { this->farClip = farClip; }
    inline void setOrthographicDimensions(float w, float h) { orthoWidth = w; orthoHeight = h; }
    inline void setAspectRatio(float aspectR) { aspectRatio = aspectR; }
    inline void setAutoAspectRatio(bool autoAspR) { autoAspectRatio = autoAspR; }
    inline void setActive(bool active) { this->active = active; }
    inline void setPriority(int priority) { this->priority = priority; }
    inline void setPolygonMode(HUGO::PolygonMode mode) { polygonMode = mode; }
    // Getters
    inline HUGO::ProjectionType getProjectionType() const { return projectionType; }
    inline float getFOVY() const { return fovY; }
    inline float getNearClip() const { return nearClip; }
    inline float getFarClip() const { return farClip; }
    inline std::pair<float, float> getOrthographicDimensions() const { return {orthoWidth, orthoHeight}; }
    inline float getAspectRatio() const { return aspectRatio; }
    inline bool getAutoAspectRatio() const { return autoAspectRatio; }
    inline bool isActive() const { return active; }
    inline int getPriority() const { return priority; }
    inline HUGO::PolygonMode getPolygonMode() const { return polygonMode; }

END_HCOMPONENT()
