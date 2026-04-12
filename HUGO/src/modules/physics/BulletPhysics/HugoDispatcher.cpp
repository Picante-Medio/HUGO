#include "HugoDispatcher.h"
#include "BulletSimulation.h"
#include "ColliderComponent.h"
#include "InternalScene.h"

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
        btCollisionObject* co0 = static_cast<btCollisionObject*>(pair.m_pProxy0->m_clientObject);
        btCollisionObject* co1 = static_cast<btCollisionObject*>(pair.m_pProxy1->m_clientObject);
        
        if(dispatcher->needsCollision(co0, co1)){
            CollisionObject* hgColObj0 = static_cast<CollisionObject*>(co0->getUserPointer());
            CollisionObject* hgColObj1 = static_cast<CollisionObject*>(co1->getUserPointer());

            EntityID hgEnt0 = hgColObj0->hgId;
            EntityID hgEnt1 = hgColObj1->hgId;
            
            // Activations to not let the objects sleep if they are colliding
            co0->activate(true);
            co1->activate(true);
    
            hgColObj0->scene->getComponent<ColliderComponent>(hgEnt0).collisionBuffer.push_back(hgEnt1);
            hgColObj1->scene->getComponent<ColliderComponent>(hgEnt1).collisionBuffer.push_back(hgEnt0);
        }
    }

}
