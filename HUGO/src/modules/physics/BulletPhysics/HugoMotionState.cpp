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

    btQuaternion q = BulletUtils::quaternionToBullet(hugoTransform.getRotation());
    q.normalize();

    // Uses associated HUGO::TransformComponent to update itself
    btTransform auxTransform = btTransform(
                               q, 
                               BulletUtils::vector3ToBullet(hugoTransform.getPosition()));

    btTransformRegister = auxTransform;
}

void HUGOMotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = btTransformRegister;
}
    
void HUGOMotionState::setWorldTransform(const btTransform& worldTrans)
{
    btTransformRegister = worldTrans;
    // if(hugoTransform.use_count() == 0){
    //     return;
    // }

    TransformComponent& hugoTransform = hugoScene->getComponent<TransformComponent>(entityID);
        
    btQuaternion rot = btTransformRegister.getRotation();
    hugoTransform.setRotation(HUGO::Quaternion(rot.w(), rot.x(), rot.y(), rot.z()));
    btVector3 pos = btTransformRegister.getOrigin();
    hugoTransform.setPosition(HUGO::Vector3(pos.x(), pos.y(), pos.z()));
}
