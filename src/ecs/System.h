#pragma once

#include <memory>

class Scene;

// Systems are only functionality
class System {
protected:
  std::shared_ptr<Scene> scene = nullptr;

public:
  virtual void update(double dt) {}
  virtual void fixedUpdate(double dt) {}
};
