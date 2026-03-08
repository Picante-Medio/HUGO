#pragma once

#include "HugoDispatcher.h"
#include "RigidBodyComponent.h"

HUGODispatcher::HUGODispatcher(btCollisionConfiguration *collisionConfiguration)
: btCollisionDispatcher(collisionConfiguration)
{}

HUGODispatcher::~HUGODispatcher()
{}

void HUGODispatcher::dispatchAllCollisionPairs(btOverlappingPairCache *pairCache, const btDispatcherInfo &dispatchInfo, btDispatcher *dispatcher)
{
    btBroadphasePairArray collisionsThisFrame = pairCache->getOverlappingPairArray();

    // btrBroadphasePairArray doesn't have a begin function
    for(int i = 0; i < collisionsThisFrame.size(); ++i){
        btBroadphasePair pair = collisionsThisFrame.at(i);
        RigidBodyComponent* hgRb0 = static_cast<RigidBodyComponent*>(static_cast<btRigidBody*>(pair.m_pProxy0->m_clientObject)->getUserPointer());
        RigidBodyComponent* hgRb1 = static_cast<RigidBodyComponent*>(static_cast<btRigidBody*>(pair.m_pProxy1->m_clientObject)->getUserPointer());
    
        // Actualizar collisionInfor de los hgRB
    }

    btCollisionDispatcher::dispatchAllCollisionPairs(pairCache, dispatchInfo, dispatcher);
}
