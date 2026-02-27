#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

// Camera component that holds all necessary information for rendering a camera in the scene
struct LightComponent: public Component
{
    HUGO::Vector4 diffuse;
    HUGO::Vector4 specular;
    HUGO::LightType lightType = HUGO::LightType::LT_POINT;

    

    LightComponent(EntityID entity,
                   float fov = 60.0f, float nearP = 0.1f, float farP = 1000.0f,
                   int prio = 0, bool isActive = true)
        : Component(entity) {}
};