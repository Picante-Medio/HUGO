#include "Scene.h"

#define HSYSTEM(SYSTEM_NAME) \
class SYSTEM_NAME : System { \
    SYSTEM_NAME(std::shared_ptr<Scene> scene): System(scene) {} \
    static inline const std::string name = #SYSTEM_NAME;

#define END_HSYSTEM() \
};