#include "HugoMotionState.h"
#include "BulletUtils.h"
#include "TransformComponent.h"

HUGOMotionState::HUGOMotionState(const btTransform initialPos, std::shared_ptr<Scene> scene, unsigned long int id, btTransform offset)
: entityID(id)
, hugoScene(scene)
, btTransformRegister(initialPos)
, centerOfMassOffset(offset)
{

}

// void HUGOMotionState::setHugoTransform(TransformComponent& transform)
// {
//     hugoTransform = transform;
// }
    
void HUGOMotionState::updateBulletTransform()
{

    TransformComponent& hugoTransform = hugoScene->getComponent<TransformComponent>(entityID);

    // Uses associated HUGO::TransformComponent to update itself
    btTransform auxTransform = btTransform(
                               BulletUtils::quaternionToBullet(hugoTransform.rotation), 
                               BulletUtils::vector3ToBullet(hugoTransform.position));

    btTransformRegister = auxTransform * centerOfMassOffset;
}

void HUGOMotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = btTransformRegister * centerOfMassOffset;
}
    
void HUGOMotionState::setWorldTransform(const btTransform& worldTrans)
{
    btTransformRegister = worldTrans * centerOfMassOffset;
    // if(hugoTransform.use_count() == 0){
    //     return;
    // }
    
    TransformComponent& hugoTransform = hugoScene->getComponent<TransformComponent>(entityID);
        
    btQuaternion rot = btTransformRegister.getRotation();
    hugoTransform.worldRotation = HUGO::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
    btVector3 pos = btTransformRegister.getOrigin();
    hugoTransform.worldPosition = HUGO::Vector3(pos.x(), pos.y(), pos.z());
}
