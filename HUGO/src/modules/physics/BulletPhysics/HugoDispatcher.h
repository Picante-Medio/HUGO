#pragma once

#include "btBulletDynamicsCommon.h"

class HUGODispatcher : public btCollisionDispatcher
{
public:

    virtual void dispatchAllCollisionPairs(btOverlappingPairCache *pairCache, const btDispatcherInfo &dispatchInfo, btDispatcher *dispatcher) override;

    HUGODispatcher(btCollisionConfiguration *collisionConfiguration);
    ~HUGODispatcher();
};