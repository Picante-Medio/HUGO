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
            if(pair.m_algorithm){
                ColliderComponent* hgCol0 = static_cast<ColliderComponent*>(co0->getUserPointer());
                ColliderComponent* hgCol1 = static_cast<ColliderComponent*>(co1->getUserPointer());
                
                // Activations to not let the objects sleep if they are colliding
                co0->activate(true);
                co1->activate(true);
    
                hgCol0->collisionInfo.push_back(hgCol1->entity);
                hgCol1->collisionInfo.push_back(hgCol0->entity);
            }
        }
    }

}
