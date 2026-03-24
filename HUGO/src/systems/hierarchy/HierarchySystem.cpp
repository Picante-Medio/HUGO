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
HierarchySystem::addChildToParent(EntityID child, EntityID parent){
    std::vector<EntityID>* children = nullptr;
    if(scene->hasComponent<ChildrenComponent>(parent)){
        children = &scene->getComponent<ChildrenComponent>(parent).children;
    }
    else{
        children = &scene->addAndGetComponent<ChildrenComponent>(parent).children;
    }
    auto it = std::find(children->begin(), children->end(), child);
    if (it == children->end()) {
        children->push_back(child);
    }
}

void 
HierarchySystem::removeChildFromParent(EntityID child, EntityID parent){
    if (!scene->hasComponent<ChildrenComponent>(parent)) return;

    std::vector<EntityID>& children = scene->getComponent<ChildrenComponent>(parent).children;
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}


void 
HierarchySystem::addParentToChild(EntityID child, EntityID parent){
    if(scene->hasComponent<ParentComponent>(child)){
        scene->getComponent<ParentComponent>(child).parent = parent;
    }
    else{
        scene->addAndGetComponent<ParentComponent>(child).parent = parent;
    }
}
void 
HierarchySystem::removeParentFromChild(EntityID child, EntityID parent){
    if(scene->hasComponent<ParentComponent>(child)){
        scene->getComponent<ParentComponent>(child).parent = INVALID_ENTITY_ID;
    }
}

bool 
HierarchySystem::hasCycle(EntityID child, EntityID newParent){
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
HierarchySystem::updateHierarchy(){    

    //update changes in ChildrenComponent
    for(auto& pack: scene->query<ChildrenCompDirtyTag,ChildrenComponent>()){
        ChildrenComponent& childrenComp = pack.getDeprecated<ChildrenComponent&>();
        EntityID parent = childrenComp.entity;
        std::vector<EntityID>& toAdd = childrenComp.addedChildren;
        std::vector<EntityID>& toRemove = childrenComp.removedChildren;
        std::vector<EntityID>& children = childrenComp.children;
        
        //child added
        if(!toAdd.empty()){
            for(EntityID& child: toAdd){
                //check if child is valid and does not create cycle
                if(child == INVALID_ENTITY_ID || child == parent || hasCycle(child, parent)){
                    std::cerr << "[HierarchySystem] Cannot add child "<< child <<" for entity " << parent << '\n';
                    continue;
                } 
                //add parentComponent to child
                addParentToChild(child, parent);  

                //check if parent has the child
                auto it = std::find(children.begin(), children.end(), child);
                if (it == children.end()) {
                    children.push_back(child);
                }
            }
            toAdd.clear();
        }

        //child removed
        if(!toRemove.empty()){
            for(EntityID& child: toRemove){
                //check if child is valid
                if(child == INVALID_ENTITY_ID || child == parent){
                    std::cerr << "[HierarchySystem] Cannot remove child "<< child <<" for entity " << parent << '\n';
                    continue;
                } 
                
                //remove the parent
                removeParentFromChild(child, parent); 
                
                //remove the child
                auto it = std::find(children.begin(), children.end(), child);
                if (it != children.end()) {
                    children.erase(it);
                }
            }
            toRemove.clear();
        }
        
        scene->removeComponent<ChildrenCompDirtyTag>(parent);
    }

    //update changes in ParentComponent
    for (auto& pack : scene->query<ParentCompDirtyTag, ParentComponent>()){
        ParentComponent& parentComp = pack.getDeprecated<ParentComponent&>();
        EntityID child = parentComp.entity;
        EntityID newParent= parentComp.parent;
        EntityID oldParent= parentComp.previousParent;

        if (newParent == child || hasCycle(child, newParent)) {
            parentComp.parent = oldParent;
            scene->removeComponent<ParentCompDirtyTag>(child);
            std::cerr << "[HierarchySystem] Cannot set parent "<< newParent << " for entity "<< child<<'\n';
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

        //update parentComponent and notify
        parentComp.previousParent = newParent;
        if (!scene->hasComponent<WorldTransformDirtyTag>(child)) {
            scene->addComponent<WorldTransformDirtyTag>(child);
            addWorldDirtyTagForChildren(child);
        }

        scene->removeComponent<ParentCompDirtyTag>(child);
    }
}

void HierarchySystem::addWorldDirtyTagForChildren(EntityID parent)
{
    std::vector<EntityID> needTag; //list of entities that need to propagate the tag
    needTag.push_back(parent);

    while(!needTag.empty())
    {
        EntityID current = needTag.back(); 
        needTag.pop_back();
        
        //if entity has children --> add tag to the children
        if (scene->hasComponent<ChildrenComponent>(current)) {
            
            const std::vector<EntityID>& children = scene->getComponent<ChildrenComponent>(current).children;
           
            for (EntityID child : children) {
                if(!scene->hasComponent<WorldTransformDirtyTag>(child)){
                    scene->addComponent<WorldTransformDirtyTag>(child);
                    needTag.push_back(child);
                }
            }
        }
    }
}

void 
HierarchySystem::setLocalTransform(TransformComponent &transform){
    transform.worldPosition = transform.position;
    transform.worldRotation = transform.rotation;
    transform.worldScale = transform.scale; 
}

void
HierarchySystem::updateWorldTransform(TransformComponent& transform){
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
    TransformComponent& parentTransform = getFullTransform(parentComp.parent);

    //rotation/orientation
    transform.worldRotation = parentTransform.worldRotation * transform.rotation;

    //scale
    transform.worldScale = parentTransform.worldScale * transform.scale;
    
    //position
    transform.worldPosition = parentTransform.worldRotation *( parentTransform.worldScale*transform.position);
    transform.worldPosition += parentTransform.worldPosition;
}

TransformComponent& 
HierarchySystem::getFullTransform(EntityID entity){

    TransformComponent& tr = scene->getComponent<TransformComponent>(entity);
    
    //check if it needs update
    if (!scene->hasComponent<WorldTransformDirtyTag>(entity))
        return tr;

    updateWorldTransform(tr);
    
    scene->removeComponent<WorldTransformDirtyTag>(entity);
    return tr;
}

void 
HierarchySystem::update(double){

    updateHierarchy();
    
    //proccess local transform changes --> add WorldTransformDirty Tag
    for (auto& pack : scene->query<TransformChangedTag, TransformComponent>()) {
        EntityID e = pack.getDeprecated<TransformComponent&>().entity;
    
        if(!scene->hasComponent<WorldTransformDirtyTag>(e))
        {
            scene->addComponent<WorldTransformDirtyTag>(e);
            addWorldDirtyTagForChildren(e);
        }
        
        scene->removeComponent<TransformChangedTag>(e);
    }

    //process dirty tag: update worldTranforms
    for (auto& pack : scene->query<WorldTransformDirtyTag, TransformComponent>()) {
        TransformComponent& tr= pack.getDeprecated<TransformComponent&>();
        getFullTransform(tr.entity);
    }

}

void 
HierarchySystem::fixedUpdate(double) {}

void HierarchySystem::onEntityDestroyed(EntityID entity){

    if(scene->hasComponent<ParentComponent>(entity)){
        removeChildFromParent(entity,scene->getComponent<ParentComponent>(entity).parent);
    }
    if (!scene->hasComponent<ChildrenComponent>(entity)) return;
    const std::vector<EntityID>& children = scene->getComponent<ChildrenComponent>(entity).children;
    for (EntityID child : children)
    {
        //decir que los hijos estan muertos tambien
    }
}
