#include "Ecs.h"

namespace ECSTesting {
  void tryToCreateEntities(int amount);

  struct TestComponent {
    EntityID entity;
    TestComponent(EntityID entity) : entity(entity) {};
  };
  void tryToAddTestComponent();

  void tryToQuery();
}
