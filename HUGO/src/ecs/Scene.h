#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "ComponentDenseTable.h"
#include "Ecs.h"
#include "EntityTable.h"
#include "HQuery.h"
#include "System.h"
#include "HugoTypes.h"
#include "LuaConsole.h"

#include "IScene.h"

class Scene : public IScene, public std::enable_shared_from_this<Scene> {
  
  std::vector<std::shared_ptr<System>> systems;
  std::unique_ptr<System> console;

public:
  Scene();

  inline void addSystem(std::shared_ptr<System> system) { systems.push_back(system); }

  // Logic update
  void update(double dt);

  // Physics update
  void fixedUpdate(double dt);

  // Entity handling
  EntityID createEntity();
  int createEntityLUA();

  void removeEntity(EntityID entity);
  void removeEntityLUA(int entity);

  void printEntities();
};
