#include "Component.h"
#include "HugoMath.h"

// Projection types for the camera
enum class ProjectionType {
    Perspective,
    Orthographic
};

// Polygon rendering modes
enum class PolygonMode {
    Solid,
    Wireframe,
    Points
};

// Camera component that holds all necessary information for rendering a camera in the scene
struct CameraComponent: public Component
{
  ProjectionType projectionType = ProjectionType::Perspective;
 
  // Perspective parameters
  float fovY; // Field of view in the Y direction, in degrees
  float nearClip;
  float farClip;

  // Orthographic parameters
  float orthoWidth = 10.f;
  float orthoHeightf = 10.f;

  //Aspect ratio
  float aspectRatio = 16.0f / 9.0f;
  bool autoAspectRatio = true;

  bool active = true;
  int priority = 0; // for multiple cameras, lower priority renders first
  
  // Modos
  PolygonMode polygonMode = PolygonMode::Solid;
  
  CameraComponent(EntityID entity,
                   float fov = 60.0f, float nearP = 0.1f, float farP = 1000.0f,
                   int prio = 0, bool isActive = true)
        : Component(entity)
        , fovY(fov)
        , nearClip(nearP)
        , farClip(farP)
        , priority(prio)
        , active(isActive) {}
};