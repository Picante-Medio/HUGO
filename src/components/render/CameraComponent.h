#include "../ecs/Component.h"
#include "../utils/HugoMath.h"

struct CameraComponent: public Component
{
    float fov;
    float nearClip;
    float farClip;
    float aspectRatio;
    bool active;

    HUGO::Vector3 position;
    HUGO::Quaternion rotation;

    CameraComponent(EntityID entity,
                    float fov = 60.0f, 
                    float nearClip = 1.0f, 
                    float farClip = 1000.0f,
                    float aspectRatio = 16.0f/9.0f, 
                    bool active = false,
                    const HUGO::Vector3& position = HUGO::Vector3(0, 0, 0),
                    const HUGO::Quaternion& rotation= HUGO::Quaternion(1,0,0,0))
        : Component(entity), fov(fov), nearClip(nearClip), farClip(farClip),
          aspectRatio(aspectRatio), active(active),
          position(position), rotation(rotation) {}
};