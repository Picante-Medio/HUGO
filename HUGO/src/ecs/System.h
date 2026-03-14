#pragma once

#include <memory>

class Scene;

// Systems are only functionality
class System {
protected:
  std::shared_ptr<Scene> scene = nullptr;

public:
  explicit System(std::shared_ptr<Scene> scene): scene(scene) {}
  virtual ~System() {}
  //Called when the System is created
  virtual void init() = 0;
  //Called when its Scene is loaded (For the first time or not)
  virtual void onLoad() =0;
  virtual void fixedUpdate(double dt) = 0;
  virtual void update(double dt) = 0;
};
