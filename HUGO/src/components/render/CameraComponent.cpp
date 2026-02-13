#pragma once
#include "CameraComponent.h"

CameraComponent::CameraComponent(EntityID entity, float fov, float nearP, float farP, int prio, bool isActive)
  : Component(entity), fovY(fov), nearClip(nearP), farClip(farP), priority(prio), active(isActive) {}

#pragma region Setters
void CameraComponent::setProjectionType(HUGO::ProjectionType type) {
  projectionType = type;
}

void CameraComponent::setFOVY(float fov) {
  fovY = fov;
}

void CameraComponent::setNearClip(float nearClip) {
  nearClip = nearClip;
}

void CameraComponent::setFarClip(float farClip) {
  farClip = farClip;
}

void CameraComponent::setOrthographicDimensions(float width, float height) {
  orthoWidth = width;
  orthoHeight = height;
}

void CameraComponent::setAspectRatio(float aspectRatio) {
  aspectRatio = aspectRatio;
}

void CameraComponent::setAutoAspectRatio(bool autoAspectRatio) {
  autoAspectRatio = autoAspectRatio;
}

void CameraComponent::setActive(bool active) {
  active = active;
}

void CameraComponent::setPriority(int priority) {
  priority = priority;
}

void CameraComponent::setPolygonMode(HUGO::PolygonMode mode) {
  polygonMode = mode;
}

#pragma endregion

#pragma region Getters

HUGO::ProjectionType CameraComponent::getProjectionType() const {
  return projectionType;
}

float CameraComponent::getFOVY() const {
  return fovY;
}

float CameraComponent::getNearClip() const {
  return nearClip;
}

float CameraComponent::getFarClip() const {
  return farClip;
}

std::pair<float, float> CameraComponent::getOrthographicDimensions() const {
  return {orthoWidth, orthoHeight};
}

float CameraComponent::getAspectRatio() const {
  return aspectRatio;
}

bool CameraComponent::getAutoAspectRatio() const {
  return autoAspectRatio;
}

bool CameraComponent::isActive() const {
  return active;
}

int CameraComponent::getPriority() const {
  return priority;
}

HUGO::PolygonMode CameraComponent::getPolygonMode() const {
  return polygonMode;
}
#pragma endregion
