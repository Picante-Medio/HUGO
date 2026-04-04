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

    if (!scene->hasComponent<ChildrenComponent>(parent)) {
        scene->addComponent<ChildrenComponent>(parent, [child](ChildrenComponent& childrenComponent) {
            childrenComponent.children.insert(child);
        });
    }
    else {
        scene->modifyComponent<ChildrenComponent>(parent, [child](ChildrenComponent& childrenComponent) {
            childrenComponent.children.insert(child);
        });
    }
}

void 
HierarchySystem::removeChildFromParent(EntityID child, EntityID parent){
    if (!scene->hasComponent<ChildrenComponent>(parent)) return;

    std::unordered_set<EntityID>& children = scene->getComponent<ChildrenComponent>(parent).children;
    children.erase(child);
}


void 
HierarchySystem::addParentToChild(EntityID child, EntityID parent){

    if (scene->hasComponent<ParentComponent>(child)) {
        scene->modifyComponent<ParentComponent>(child, [parent](ParentComponent& parentComp){
            parentComp.previousParent = parentComp.parent;
            parentComp.parent = parent;
        });
    }
    else {
        scene->addComponent<ParentComponent>(child, [parent](ParentComponent& parentComp){
            parentComp.parent = parent;
        });
    }
}
void 
HierarchySystem::removeParentFromChild(EntityID child){
    if(scene->hasComponent<ParentComponent>(child)){
        scene->getComponent<ParentComponent>(child).parent.reset();   
    }
}

bool 
HierarchySystem::hasCycle(EntityID child,EntityID newParent){
    std::optional<EntityID> current = newParent;
    while (current)
    {
        if (current == child) return true;
        if (!scene->hasComponent<ParentComponent>(current.value())) break;
        current = scene->getComponent<ParentComponent>(current.value()).parent;
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

        //child added
        if(!toAdd.empty()){
            for(EntityID& child: toAdd){
                //check if child is valid and does not create cycle
                if(child == parent || hasCycle(child, parent)){
                    std::cerr << "[HierarchySystem] Cannot add child "<< child <<" for entity " << parent << '\n';
                    std::unordered_set<EntityID>& children = childrenComp.children;
                    children.erase(child);
                    continue;
                } 
                //add parentComponent to child
                addParentToChild(child, parent);  
            }
            toAdd.clear();
        }

        //child removed
        if(!toRemove.empty()){
            for(EntityID& child: toRemove){
                //check if child is valid
                if(child == parent){
                    std::cerr << "[HierarchySystem] Cannot remove child "<< child <<" for entity " << parent << '\n';
                    continue;
                } 
                
                //remove the parent
                removeParentFromChild(child); 
            }
            toRemove.clear();
        }
        
        scene->removeComponent<ChildrenCompDirtyTag>(parent);
    }

    //update changes in ParentComponent
    for (auto& pack : scene->query<ParentCompDirtyTag, ParentComponent>()){
        ParentComponent& parentComp = pack.getDeprecated<ParentComponent&>();
        
        if(!parentComp.parent) 
            continue;
        EntityID child = parentComp.entity;
        EntityID newParent= parentComp.parent.value();
        std::optional<EntityID> oldParent= parentComp.previousParent;

        if (newParent == child || hasCycle(child, newParent)) {
            if(oldParent)parentComp.parent = oldParent;
            scene->removeComponent<ParentCompDirtyTag>(child);
            std::cerr << "[HierarchySystem] Cannot set parent "<< newParent << " for entity "<< child<<'\n';
            continue;
        }

         //remove child from old parent
        if(oldParent){
            removeChildFromParent(child, oldParent.value());
        }

        //add child in new parent
        addChildToParent(child, newParent);

        //update parentComponent
        parentComp.previousParent = newParent;
        scene->removeComponent<ParentCompDirtyTag>(child);
    }
}


void 
HierarchySystem::setWorldTransform(TransformComponent &transform){
    if(transform.needUpdateLocalPosition) transform.position = transform.worldPosition;
    if(transform.needUpdateLocalRotation) transform.rotation = transform.worldRotation;
    if(transform.needUpdateLocalScale) transform.scale = transform.worldScale;
}

void 
HierarchySystem::updateLocalTransform(TransformComponent &transform){
    EntityID entity = transform.entity;

    // if the entity does not has parent, set it's world transform
    if(!scene->hasComponent<ParentComponent>(entity)){
        setWorldTransform(transform);
        return;
    }

    ParentComponent& parentComp = scene->getComponent<ParentComponent>(entity);
    if(!parentComp.parent){ //parent does not exist
        setWorldTransform(transform);
        return;
    }

    //if entity has a parent, the transform changes according to the parent
    TransformComponent& parentTransform = getFullTransform(parentComp.parent.value());

    //position
    if(transform.needUpdateLocalPosition){
        HUGO::Vector3 pos = transform.worldPosition - parentTransform.worldPosition;
        transform.position = parentTransform.worldRotation.inverse() * pos / parentTransform.worldScale;
    } 

    //rotation/orientation
    if(transform.needUpdateLocalRotation) transform.rotation = parentTransform.worldRotation.inverse() * transform.worldRotation;

    //scale
    if(transform.needUpdateLocalScale) transform.scale = transform.worldScale / parentTransform.worldScale;
}


void 
HierarchySystem::setLocalTransform(TransformComponent &transform){
    if(transform.needUpdateWorldPosition) transform.worldPosition = transform.position;
    if(transform.needUpdateWorldRotation) transform.worldRotation = transform.rotation;
    if(transform.needUpdateWorldScale) transform.worldScale = transform.scale;
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
    if(!parentComp.parent){
        setLocalTransform(transform);
        return;
    }

    //if entity has a parent, the transform changes according to the parent
    TransformComponent& parentTransform = getFullTransform(parentComp.parent.value());

    //rotation/orientation
    if(transform.needUpdateWorldRotation) transform.worldRotation = parentTransform.worldRotation * transform.rotation;

    //scale
    if(transform.needUpdateWorldScale) transform.worldScale = parentTransform.worldScale * transform.scale;
    
    //position
    if(transform.needUpdateWorldPosition) 
    {
        transform.worldPosition = parentTransform.worldRotation *( parentTransform.worldScale*transform.position);
        transform.worldPosition += parentTransform.worldPosition;
    }
}

TransformComponent& 
HierarchySystem::getFullTransform(EntityID entity){

    TransformComponent& tr = scene->getComponent<TransformComponent>(entity);
    
    //check if the entity needs update
     if(tr.needUpdateLocalPosition || tr.needUpdateLocalRotation || tr.needUpdateLocalScale){
        updateLocalTransform(tr);
        tr.needUpdateLocalPosition = false;
        tr.needUpdateLocalRotation = false;
        tr.needUpdateLocalScale = false;
    }
    
    if(tr.needUpdateWorldPosition || tr.needUpdateWorldRotation || tr.needUpdateWorldScale) {
        updateWorldTransform(tr);
        tr.needUpdateWorldPosition = false;
        tr.needUpdateWorldRotation = false;
        tr.needUpdateWorldScale = false;
    }
    
    return tr;
}

void HierarchySystem::propagateNeedUpdateToChildren(EntityID parent)
{
    std::vector<EntityID> needPropagate; //list of entities that need to propagate the tag
    needPropagate.push_back(parent);

    while(!needPropagate.empty())
    {
        EntityID current = needPropagate.back(); 
        needPropagate.pop_back();
        
        //if entity has children --> the children needUpdate
        if (scene->hasComponent<ChildrenComponent>(current)) {
            
            const std::unordered_set<EntityID>& children = scene->getComponent<ChildrenComponent>(current).children;
            for (EntityID child : children) {
                TransformComponent& tr = scene->getComponent<TransformComponent>(child);
                if(!(tr.needUpdateWorldPosition || tr.needUpdateWorldRotation || tr.needUpdateWorldScale)) needPropagate.push_back(child);
                tr.needUpdateWorldPosition = true;
                tr.needUpdateWorldRotation = true;
                tr.needUpdateWorldScale = true;
            }
        }
    }
}

void 
HierarchySystem::update(double){

    updateHierarchy();
    
    //update needUpdate state
    for (auto& pack : scene->query<TransformComponent>()) {
        TransformComponent& tr= pack.getDeprecated<TransformComponent&>();
        if(tr.needUpdateWorldPosition || tr.needUpdateWorldRotation || tr.needUpdateWorldScale){
            propagateNeedUpdateToChildren(tr.entity);
        }
    }

    //update worldTranforms
    for (auto& pack : scene->query<TransformComponent>()) {
        TransformComponent& tr= pack.getDeprecated<TransformComponent&>();
        getFullTransform(tr.entity);
    }
}

void 
HierarchySystem::fixedUpdate(double) {}
