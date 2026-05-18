#pragma once
#include <memory>
#include "btBulletDynamicsCommon.h"

class BulletSimulation;

class HUGODispatcher : public btCollisionDispatcher {
    BulletSimulation* simulation;
public:

    virtual void dispatchAllCollisionPairs(btOverlappingPairCache *pairCache, const btDispatcherInfo &dispatchInfo, btDispatcher *dispatcher) override;

    HUGODispatcher(btCollisionConfiguration *collisionConfiguration, BulletSimulation* simulation);
    ~HUGODispatcher();
};