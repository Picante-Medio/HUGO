#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

// Camera component that holds all necessary information for rendering a camera in the scene
HCOMPONENT(CameraComponent)

private:

    /*
        =========== CAMERA PARAMETERS =================
    */
    DEFAULT_SERIALIZED_MEMBER(HUGO::ProjectionType, projectionType, HUGO::ProjectionType::Perspective);
    
    // Perspective parameters
    // Field of view in the Y direction, in degrees
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, fovY, 60.0f) 
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, nearClip, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, farClip, 10000.0f);

    // Orthographic parameters
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, orthoWidth, 10.f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, orthoHeight, 10.f);

    //Aspect ratio
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, autoAspectRatio, true);

    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, mainCamera, false);

    /*
        =========== VIEWPORT PARAMETERS ===============
    */
    // Background color and clear options
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, clearColor, HUGO::Vector4(0.3f, 0.3f, 0.5f, 1.0f));
  
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
    inline void setMain(bool boolean){mainCamera = boolean;}
    // Getters
    inline HUGO::ProjectionType getProjectionType() const { return projectionType; }
    inline float getFOVY() const { return fovY; }
    inline float getNearClip() const { return nearClip; }
    inline float getFarClip() const { return farClip; }
    inline std::pair<float, float> getOrthographicDimensions() const { return {orthoWidth, orthoHeight}; }
    inline bool getAutoAspectRatio() const { return autoAspectRatio; }
    inline bool isMain()const{return mainCamera;}

    /*
        =========== VIEWPORT METHODS ============
    */
    // Setters
    inline void setClearColor(const HUGO::Vector4& color) { clearColor = color; }
    // Getters
    inline HUGO::Vector4 getClearColor() const { return clearColor; }

END_HCOMPONENT()

