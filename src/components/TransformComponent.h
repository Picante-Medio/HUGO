#include "../ecs/Component.h"
#include "../math/HugoMath.h"

struct TransformComponent: public Component
{
    HUGO::Vector3 position;
    HUGO::Quaternion rotation;
    HUGO::Vector3 scale;

    TransformComponent(EntityID entity,
                        HUGO::Vector3& const position = HUGO::Vector3(0, 0, 0),
                        HUGO::Quaternion& const rotation= HUGO::Quaternion(1,0,0,0),
                        HUGO::Vector3& const scale = HUGO::Vector3(1, 1, 1)): 
                        Component(entity), position(position), rotation(rotation), scale(scale) {}
};