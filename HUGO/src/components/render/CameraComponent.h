#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

// Camera component that holds all necessary information for rendering a camera in the scene
struct CameraComponent: public Component
{
private:
  HUGO::ProjectionType projectionType = HUGO::ProjectionType::Perspective;
 
  // Perspective parameters
  float fovY; // Field of view in the Y direction, in degrees
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
  CameraComponent(EntityID entity,
                  float fov = 60.0f, float nearP = 0.1f, float farP = 1000.0f,
                  int prio = 0, bool isActive = true);

  // Setters
  void setProjectionType(HUGO::ProjectionType type);
  void setFOVY(float fov);
  void setNearClip(float nearClip);
  void setFarClip(float farClip);
  void setOrthographicDimensions(float width, float height);
  void setAspectRatio(float aspectRatio);
  void setAutoAspectRatio(bool autoAspectRatio);
  void setActive(bool active);
  void setPriority(int priority);
  void setPolygonMode(HUGO::PolygonMode mode);

  // Getters
  HUGO::ProjectionType getProjectionType() const;
  float getFOVY() const;
  float getNearClip() const;
  float getFarClip() const;
  std::pair<float, float> getOrthographicDimensions() const;
  float getAspectRatio() const;
  bool getAutoAspectRatio() const;
  bool isActive() const;
  int getPriority() const;
  HUGO::PolygonMode getPolygonMode() const;
};