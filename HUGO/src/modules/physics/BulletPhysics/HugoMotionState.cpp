#include "HugoMotionState.h"
#include "BulletUtils.h"


HUGOMotionState::HUGOMotionState(const btTransform initialPos, std::shared_ptr<TransformComponent> transform, btTransform offset)
: btTransformRegister(initialPos)
, hugoTransform(transform)
, centerOfMassOffset(offset)
{

}

void HUGOMotionState::setHugoTransform(std::shared_ptr<TransformComponent> transform)
{
    hugoTransform = transform;
}
    
void HUGOMotionState::updateBulletTransform()
{
    // Uses associated HUGO::TransformComponent to update itself
    btTransform auxTransform = btTransform(
                               BulletUtils::quaternionToBullet(hugoTransform->rotation), 
                               BulletUtils::vector3ToBullet(hugoTransform->position));

    btTransformRegister = auxTransform * centerOfMassOffset;
}

void HUGOMotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = btTransformRegister * centerOfMassOffset;
}
    
void HUGOMotionState::setWorldTransform(const btTransform& worldTrans)
{
    btTransformRegister = worldTrans * centerOfMassOffset;
    if(hugoTransform.use_count() == 0){
        return;
    }

    btQuaternion rot = btTransformRegister.getRotation();
    hugoTransform->rotation = HUGO::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
    btVector3 pos = btTransformRegister.getOrigin();
    hugoTransform->position = HUGO::Vector3(pos.x(), pos.y(), pos.z());
}
