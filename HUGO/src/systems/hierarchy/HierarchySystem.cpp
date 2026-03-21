#include "HierarchySystem.h"
#include <algorithm>

#include "Scene.h"
#include "TransformComponent.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"
#include "HierarchyTags.h"

void 
HierarchySystem::init(){}

void 
HierarchySystem::onLoad(){}

void 
HierarchySystem::addChildToParent(EntityID child, EntityID parent)
{
    ChildrenComponent& childrenComp = scene->getComponent<ChildrenComponent>(parent);
    auto it = std::find(childrenComp.children.begin(), childrenComp.children.end(), child);
    if (it == childrenComp.children.end()) {
        childrenComp.children.push_back(child);
    }
}

void 
HierarchySystem::removeChildFromParent(EntityID child, EntityID parent)
{
    if (!scene->hasComponent<ChildrenComponent>(parent)) return;

    std::vector<EntityID>& children = scene->getComponent<ChildrenComponent>(parent).children;
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}

bool 
HierarchySystem::hasCycle(EntityID child, EntityID newParent)
{
    if (newParent == INVALID_ENTITY_ID) return false;

    EntityID current = newParent;
    while (current != INVALID_ENTITY_ID)
    {
        if (current == child) return true;
        if (!scene->hasComponent<ParentComponent>(current)) break;
        current = scene->getComponent<ParentComponent>(current).parent;
    }

    return false;
}

void 
HierarchySystem::updateHierarchy()
{
    //change child


    //change parent
    for (auto& pack : scene->query<ParentComponent, ParentCompDirtyTag>())
    {
        ParentComponent& parentComp = pack.get<ParentComponent&>();
        EntityID child = parentComp.entity;
        EntityID newParent= parentComp.parent;
        EntityID oldParent= parentComp.previousParent;

        if (newParent == child) {
            parentComp.parent = oldParent;
            scene->removeComponent<ParentCompDirtyTag>(child);
            std::cerr << "[HierarchySystem] Cannot set parent to itself for entity " << child << '\n';
            continue;
        }
        //detect if can create a cycle 
        if(hasCycle(child, newParent)){
            parentComp.parent = oldParent;
            scene->removeComponent<ParentCompDirtyTag>(child);
            std::cerr << "[HierarchySystem] Cannot set parent "<< newParent << "for entity "<< child<<'\n';
            continue;
        }

        //remove child from old parent
        if(oldParent != INVALID_ENTITY_ID){
            removeChildFromParent(child, oldParent);
        }

        //add child in new parent
        if(newParent != INVALID_ENTITY_ID){
            addChildToParent(child, newParent);
        }
        parentComp.previousParent = newParent;

        scene->addComponent<WorldTransformDirtyTag>(child);
        
        addWorldDirtyTagForChildren(child);

        scene->removeComponent<ParentCompDirtyTag>(child);
    }
    
}

void HierarchySystem::addWorldDirtyTagForChildren(EntityID parent)
{
    std::vector<EntityID> needTag;
    std::unordered_set<EntityID> visited;
    needTag.push_back(parent);
    visited.insert(parent);

    while(!needTag.empty())
    {
        EntityID current = needTag.back(); 
        needTag.pop_back();
        
        if (scene->hasComponent<ChildrenComponent>(current)) {
            
            const std::vector<EntityID>& children = scene->getComponent<ChildrenComponent>(current).children;
           
            for (EntityID child : children) {
                if (visited.count(child)) {
                    std::cerr << "[HierarchySystem] Cycle detected in hierarchy for entity " << child << '\n';
                    continue;
                }
                if(!scene->hasComponent<WorldTransformDirtyTag>(child)){
                    scene->addComponent<WorldTransformDirtyTag>(child);
                    needTag.push_back(child);
                    visited.insert(child);
                }
            }
        }
    }
}

void 
HierarchySystem::setLocalTransform(TransformComponent &transform)
{
    transform.worldPosition = transform.position;
    transform.worldRotation = transform.rotation;
    transform.worldScale = transform.scale; 
}

void
HierarchySystem::updateWorldTransform(TransformComponent& transform, std::unordered_set<EntityID>& visited)
{
    EntityID entity = transform.entity;
    // if the entity does not has parent, set it's local trasnform
    if(!scene->hasComponent<ParentComponent>(entity)){
        setLocalTransform(transform);
        return;
    }

    ParentComponent& parentComp = scene->getComponent<ParentComponent>(entity);

    if(parentComp.parent == INVALID_ENTITY_ID){
        setLocalTransform(transform);
        return;
    }

    //if entity has a parent, the transform changes according to the parent
    TransformComponent& parentTransform = getFullTransform(parentComp.parent, visited);

    //rotation/orientation
    transform.worldRotation = parentTransform.worldRotation * transform.rotation;

    //scale
    transform.worldScale = parentTransform.worldScale * transform.scale;
    
    //position need update after scale
    transform.worldPosition = parentTransform.worldRotation *( parentTransform.worldScale*transform.position);
    transform.worldPosition += parentTransform.worldPosition;
}

TransformComponent& 
HierarchySystem::getFullTransform(EntityID entity, std::unordered_set<EntityID>& visited)
{
    TransformComponent& tr = scene->getComponent<TransformComponent>(entity);
    if(visited.count(entity)){
        std::cerr << "[HierarchySystem] Cycle detected in hierarchy for entity " << entity <<"\n";
        return tr;
    }
    visited.insert(entity);
    
    if (!scene->hasComponent<WorldTransformDirtyTag>(entity))
        return tr;

    updateWorldTransform(tr, visited);
    scene->removeComponent<WorldTransformDirtyTag>(entity);
    visited.erase(entity);
    return tr;
}

void 
HierarchySystem::update(double)
{
    updateHierarchy();
    
    //proccess local transform changes --> add WorldTransformDirty Tag
    for (auto& pack : scene->query<TransformChangedTag, TransformComponent>()) {
        EntityID e = pack.get<TransformComponent&>().entity;
    
        if(!scene->hasComponent<WorldTransformDirtyTag>(e))
        {
            scene->addComponent<WorldTransformDirtyTag>(e);
            addWorldDirtyTagForChildren(e);
        }
        
        scene->removeComponent<TransformChangedTag>(e);
    }

    //process dirty tag: update worldTranforms
    for (auto& pack : scene->query<WorldTransformDirtyTag, TransformComponent>()) {
        TransformComponent& tr= pack.get<TransformComponent&>();
        std::unordered_set<EntityID> visited; 
        getFullTransform(tr.entity, visited);
    }

}

void 
HierarchySystem::fixedUpdate(double) {}
