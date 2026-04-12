#include "HierarchySystem.h"
#include <algorithm>

#include "Scene.h"
#include "TransformComponent.h"
#include "ParentComponent.h"
#include "ChildrenComponent.h"

void 
HierarchySystem::init()
{
    scene.onAddComponent<ChildrenComponent>().connect(this, &HierarchySystem::onUpdateChildrenComponent);
    scene.onAddComponent<ParentComponent>().connect(this, &HierarchySystem::onUpdateParentComponent);
    scene.onAddComponent<TransformComponent>().connect(this, &HierarchySystem::onAddTransformComponent);
    
    scene.onUpdateComponent<ChildrenComponent>().connect(this, &HierarchySystem::onUpdateChildrenComponent);
    scene.onUpdateComponent<ParentComponent>().connect(this, &HierarchySystem::onUpdateParentComponent);

    scene.onRemoveComponent<ChildrenComponent>().connect(this, &HierarchySystem::onRemoveChildrenComponent); 
    scene.onRemoveComponent<ParentComponent>().connect(this, &HierarchySystem::onRemoveParentComponent);
}

void 
HierarchySystem::onLoad(){}

void 
HierarchySystem::addChildToParent(EntityID child, EntityID parent){

    if (!scene.hasComponent<ChildrenComponent>(parent)) {
        scene.addComponent<ChildrenComponent>(parent, [child](ChildrenComponent& childrenComponent) {
            childrenComponent.children.insert(child);
        });
    }
    else {
        scene.modifyComponent<ChildrenComponent>(parent, [child](ChildrenComponent& childrenComponent) {
            childrenComponent.children.insert(child);
        });
    }
}

void 
HierarchySystem::removeChildFromParent(EntityID child, EntityID parent){
    if (!scene.hasComponent<ChildrenComponent>(parent)) return;

    std::unordered_set<EntityID>& children = scene.getComponent<ChildrenComponent>(parent).children;
    children.erase(child);
}


void 
HierarchySystem::addParentToChild(EntityID child, EntityID parent){

    if (scene.hasComponent<ParentComponent>(child)) {
        ParentComponent& parentComp= scene.getComponent<ParentComponent>(child);
        parentComp.previousParent = parentComp.parent;
        parentComp.parent = parent;
    }
    else {
        scene.addComponent<ParentComponent>(child, [parent](ParentComponent& parentComp){
            parentComp.parent = parent;
        });
    }
}
void 
HierarchySystem::removeParentFromChild(EntityID child){
    if(scene.hasComponent<ParentComponent>(child)){
        scene.getComponent<ParentComponent>(child).parent.reset();   
    }
}

bool 
HierarchySystem::hasCycle(EntityID child,EntityID newParent){
    std::optional<EntityID> current = newParent;
    while (current)
    {
        if (current == child) return true;
        if (!scene.hasComponent<ParentComponent>(current.value())) break;
        current = scene.getComponent<ParentComponent>(current.value()).parent;
    }

    return false;
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
    if(!scene.hasComponent<ParentComponent>(entity)){
        setWorldTransform(transform);
        return;
    }

    ParentComponent& parentComp = scene.getComponent<ParentComponent>(entity);
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
    if(!scene.hasComponent<ParentComponent>(entity)){
        setLocalTransform(transform);
        return;
    }

    ParentComponent& parentComp = scene.getComponent<ParentComponent>(entity);
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

    TransformComponent& tr = scene.getComponent<TransformComponent>(entity);
    
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
        if (scene.hasComponent<ChildrenComponent>(current)) {
            
            const std::unordered_set<EntityID>& children = scene.getComponent<ChildrenComponent>(current).children;
            for (EntityID child : children) {
                TransformComponent& tr = scene.getComponent<TransformComponent>(child);
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

    //update needUpdate state
    for (auto& pack : scene.query<TransformComponent>()) {
        TransformComponent& tr= pack.getConst<TransformComponent&>();
        if(tr.needUpdateWorldPosition || tr.needUpdateWorldRotation || tr.needUpdateWorldScale
            ||tr.needUpdateLocalPosition || tr.needUpdateLocalRotation || tr.needUpdateLocalScale){
            propagateNeedUpdateToChildren(tr.entity);
        }
    }

    //update worldTranforms
    for (auto& pack : scene.query<TransformComponent>()) {
        TransformComponent& tr= pack.getConst<TransformComponent&>();
        getFullTransform(tr.entity);
    }
}

void 
HierarchySystem::fixedUpdate(double) {}


void 
HierarchySystem::onAddTransformComponent(TransformComponent& tr){
    tr.needUpdateLocalPosition = tr.worldPosition!=HUGO::Vector3();
    tr.needUpdateLocalRotation = tr.worldRotation!=HUGO::Quaternion();
    tr.needUpdateLocalScale = tr.worldScale!=HUGO::Vector3();
    getFullTransform(tr.entity);
}

void 
HierarchySystem::onUpdateChildrenComponent(ChildrenComponent& childrenComp)
{
    EntityID parent = childrenComp.entity;
    std::vector<EntityID>& toAdd = childrenComp.addedChildren;
    std::vector<EntityID>& toRemove = childrenComp.removedChildren;

    //child added
    if(!toAdd.empty()){
        for(EntityID& child: toAdd){
            //check if child is valid and does not create cycle
            if(child == parent || hasCycle(child, parent)){
                scene.log(std::make_shared<StringError>("[HierarchySystem] Cannot add child "+ std::to_string(child) + " for entity " + std::to_string(parent)));
                childrenComp.children.erase(child);
                return;
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
                scene.log(std::make_shared<StringError>("[HierarchySystem] Cannot remove child "+ std::to_string(child) + " for entity " + std::to_string(parent)));
                return;
            } 
            
            //remove the parent
            removeParentFromChild(child); 
        }
        toRemove.clear();
    }
}

void 
HierarchySystem::onUpdateParentComponent(ParentComponent& parentComp)
{
    if(!parentComp.parent)
         return;
    EntityID child = parentComp.entity;
    EntityID newParent= parentComp.parent.value();
    std::optional<EntityID> oldParent= parentComp.previousParent;

    if (newParent == child || hasCycle(child, newParent)) {
        if(oldParent)parentComp.parent = oldParent;
        scene.log(std::make_shared<StringError>("[HierarchySystem] Cannot set parent "+ std::to_string(newParent) + " for entity " + std::to_string(child)));
        return;
    }

    //remove child from old parent
    if(oldParent){
        removeChildFromParent(child, oldParent.value());
    }

    //add child in new parent
    addChildToParent(child, newParent);

    //update parentComponent
    parentComp.previousParent = newParent;
}

void 
HierarchySystem::onRemoveChildrenComponent(ChildrenComponent& childrenComp)
{
    std::unordered_set<EntityID>& children = childrenComp.children;
    while(!children.empty()){
        auto it = children.begin();
        EntityID child = *it;
        children.erase(it);
        
        //remove the parent from the child
        removeParentFromChild(child); 
    }
}

void 
HierarchySystem::onRemoveParentComponent(ParentComponent& parentComp)
{
    EntityID child = parentComp.entity;
    std::optional<EntityID> parent = parentComp.parent;
    if(parent)removeChildFromParent(child, parent.value());
}