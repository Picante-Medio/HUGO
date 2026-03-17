#pragma once

#include "System.h"
#include "HugoTypes.h"

#include "IScene.h"

class Scene : public IScene, public std::enable_shared_from_this<Scene> {
  
  std::vector<std::shared_ptr<System>> systems;

public:
  Scene();
  
  void load();

  inline void addSystem(std::shared_ptr<System> system) { systems.push_back(system); }

  // Logic update
  void update(double dt);

  // Physics update
  void fixedUpdate(double dt);

  // Entity handling
  EntityID createEntity();

  // LUA console stuff
  int createEntityLUA();

  void addComponentLUA(int entityID, std::string& componentName);
  void removeComponentLUA(int entityID, std::string& componentName);

  void removeEntity(EntityID entity);
  void removeEntityLUA(int entity);

  void printEntity(int id);
  void printEntities();
};
