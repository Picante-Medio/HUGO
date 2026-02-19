#include "mainTest.h"
#include "Scene.h"

#include <iostream>

void 
ECSTesting::tryToCreateEntities(int amount) {
  Scene hugoScene = Scene();

  for(int i = 0; i < amount; ++i) {
    EntityID id = hugoScene.createEntity();
    std::cout << "just created an entity with id " << id << "\n";
  }
}

void 
ECSTesting::tryToAddTestComponent() {
  Scene hugoScene = Scene();
  EntityID id = hugoScene.createEntity();

  hugoScene.addComponent<ECSTesting::TestComponent>(id);
  std::cout << "Added test component succesfully\n";
}


void 
ECSTesting::tryToQuery() {
  Scene hugoScene = Scene();
  EntityID id = hugoScene.createEntity();

  hugoScene.addComponent<ECSTesting::TestComponent>(id);

  int count = 0;
  for(auto pack : hugoScene.query<ECSTesting::TestComponent>()) 
    count ++;

  std::cout << "There are " << count << " entities with the testComponent\n";
}
