#include "HugoDispatcher.h"
#include "ColliderComponent.h"

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
            ColliderComponent* hgCol0 = static_cast<ColliderComponent*>(co0->getUserPointer());
            ColliderComponent* hgCol1 = static_cast<ColliderComponent*>(co1->getUserPointer());

            EntityID hgEnt0 = hgCol0->entity;
            EntityID hgEnt1 = hgCol1->entity;
            
            // Activations to not let the objects sleep if they are colliding
            co0->activate(true);
            co1->activate(true);
    
            hgCol0->collisionBuffer.push_back(hgEnt1);
            hgCol1->collisionBuffer.push_back(hgEnt0);
        }
    }

}
