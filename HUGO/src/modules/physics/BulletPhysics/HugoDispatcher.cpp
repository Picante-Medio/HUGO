#include "HugoDispatcher.h"
#include "RigidBodyComponent.h"

HUGODispatcher::HUGODispatcher(btCollisionConfiguration *collisionConfiguration)
: btCollisionDispatcher(collisionConfiguration)
{}

HUGODispatcher::~HUGODispatcher()
{}

void HUGODispatcher::dispatchAllCollisionPairs(btOverlappingPairCache *pairCache, const btDispatcherInfo &dispatchInfo, btDispatcher *dispatcher)
{
    btCollisionDispatcher::dispatchAllCollisionPairs(pairCache, dispatchInfo, dispatcher);
    
    btBroadphasePairArray collisionsThisFrame = pairCache->getOverlappingPairArray();

    // btrBroadphasePairArray doesn't have a begin function
    for(int i = 0; i < collisionsThisFrame.size(); ++i){
        btBroadphasePair pair = collisionsThisFrame.at(i);
        btRigidBody* rb0 = static_cast<btRigidBody*>(pair.m_pProxy0->m_clientObject);
        btRigidBody* rb1 = static_cast<btRigidBody*>(pair.m_pProxy1->m_clientObject);

        if(dispatcher->needsCollision(rb0, rb1)){
            if(pair.m_algorithm){
                RigidBodyComponent* hgRb0 = static_cast<RigidBodyComponent*>(rb0->getUserPointer());
                RigidBodyComponent* hgRb1 = static_cast<RigidBodyComponent*>(rb1->getUserPointer());
                
                // Activations to not let the objects sleep if they are colliding
                rb0->activate(true);
                rb1->activate(true);
    
                hgRb0->collisionInfo.push_back(hgRb1->entity);
                hgRb1->collisionInfo.push_back(hgRb0->entity);
            }
        }
    }

}
