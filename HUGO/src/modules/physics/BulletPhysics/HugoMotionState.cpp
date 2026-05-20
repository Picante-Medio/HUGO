#include "HugoMotionState.h"
#include "BulletUtils.h"
#include "WorldTransformComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"

HUGOMotionState::HUGOMotionState(const btTransform initialPos, InternalScene& scene, unsigned long int id, btTransform offset)
: entityID(id)
, hugoScene(scene)
, btTransformRegister(initialPos)
, centerOfMassOffset(offset)
{

}
    
void HUGOMotionState::updateBulletTransform()
{

    const WorldTransformComponent& hugoTransform = hugoScene.getComponentConst<WorldTransformComponent>(entityID);
    ColliderComponent hugoCol = hugoScene.getComponentConst<ColliderComponent>(entityID);

    btQuaternion q = BulletUtils::quaternionToBullet(hugoTransform.worldRotation) * BulletUtils::quaternionToBullet(hugoCol.getRotationOffset());
    q.normalize();

    // Uses associated HUGO::TransformComponent to update itself
    btTransformRegister = btTransform(q, BulletUtils::vector3ToBullet(hugoTransform.worldPosition) + BulletUtils::vector3ToBullet(hugoCol.getPositionOffset()));
}

void HUGOMotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = btTransformRegister;
}
    
void HUGOMotionState::setWorldTransform(const btTransform& worldTrans)
{
    btTransformRegister = worldTrans;

    if(!hugoScene.hasComponent<ColliderComponent>(entityID))
        return;

    const ColliderComponent& hugoCol = hugoScene.getComponentConst<ColliderComponent>(entityID);
    const RigidBodyComponent& hugoRB = hugoScene.getComponentConst<RigidBodyComponent>(entityID);
    
    hugoScene.modifyComponent<WorldTransformComponent>(entityID, [hugoRB, hugoCol, this]
        (WorldTransformComponent& wTr){
            if(!hugoRB.dontRotateWithPhysicsOnDynamic){
                btQuaternion rot = btTransformRegister.getRotation();
                wTr.worldRotation = 
                    hugoCol.getRotationOffset().inverse() * 
                    HUGO::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
            }
            btVector3 pos = btTransformRegister.getOrigin();
            wTr.worldPosition = HUGO::Vector3(pos.x(), pos.y(), pos.z()) - hugoCol.getPositionOffset();
        }
    );
}
