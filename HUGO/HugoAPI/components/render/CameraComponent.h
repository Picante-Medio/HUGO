#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

// Camera component that holds all necessary information for rendering a camera in the scene
HCOMPONENT(CameraComponent)

private:

    /*
        =========== CAMERA PARAMETERS =================
    */
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

    /*
        =========== VIEWPORT PARAMETERS ===============
    */
    // Coordinates reletad to the window (0-1)
    float left = 0.0f;
    float top = 0.0f;
    float width = 1.0f;
    float height = 1.0f;
    
    // Render order (lower renders first)
    int zOrder = 0;
    
    // Background color and clear options
    HUGO::Vector4 clearColor = HUGO::Vector4(0.3f, 0.3f, 0.5f, 1.0f);
  
public:

    /*
        =========== CAMERA METHODS ============
    */
    // Setters
    inline void setProjectionType(HUGO::ProjectionType type) { projectionType = type; }
    inline void setFOVY(float fov) { fovY = fov; }
    inline void setNearClip(float nearC) { nearClip = nearC; }
    inline void setFarClip(float farC) { farClip = farC; }
    inline void setOrthographicDimensions(float w, float h) { orthoWidth = w; orthoHeight = h; }
    inline void setAspectRatio(float aspectR) { aspectRatio = aspectR; }
    inline void setAutoAspectRatio(bool autoAspR) { autoAspectRatio = autoAspR; }
    inline void setActive(bool act) { active = act; }
    inline void setPriority(int prio) { priority = prio; }
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

    /*
        =========== VIEWPORT METHODS ============
    */
    // Setters
    inline void setLeft(float l) { left = l; }
    inline void setTop(float t) { top = t; }
    inline void setWidth(float w) { width = w; }
    inline void setHeight(float h) { height = h; }
    inline void setZOrder(int z) { zOrder = z; }
    inline void setClearColor(const HUGO::Vector4& color) { clearColor = color; }
    // Getters
    inline float getLeft() const { return left; }
    inline float getTop() const { return top; }
    inline float getWidth() const { return width; }
    inline float getHeight() const { return height; }
    inline int getZOrder() const { return zOrder; }
    inline HUGO::Vector4 getClearColor() const { return clearColor; }

END_HCOMPONENT()

