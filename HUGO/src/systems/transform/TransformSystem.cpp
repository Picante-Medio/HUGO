#include "TransformSystem.h"

#include "ParentComponent.h"
#include "TransformComponent.h"
#include "WorldTransformComponent.h"

#include <algorithm>

static constexpr size_t INVALID_NODE = std::numeric_limits<size_t>::max();

struct TransformSystem::TreeNode {

    EntityID entity;

    HUGO::Vector3 worldPosition = HUGO::Vector3();
    HUGO::Quaternion worldRotation = HUGO::Quaternion();
    HUGO::Vector3 worldScale = HUGO::Vector3(1.);

    bool dirty = false;

    size_t parent = INVALID_NODE;
    size_t firstChild = INVALID_NODE;
    size_t previousSibling = INVALID_NODE;
    size_t nextSibling = INVALID_NODE;
};

void 
TransformSystem::init() {

    scene.onAddComponent<TransformComponent>().connect(this, &TransformSystem::addTransform);
    scene.onUpdateComponent<TransformComponent>().connect(this, &TransformSystem::updateTransform);
    scene.onRemoveComponent<TransformComponent>().connect(this, &TransformSystem::removeTransform);

    scene.onAddComponent<WorldTransformComponent>().connect(this, &TransformSystem::addWorldTransform);
    scene.onUpdateComponent<WorldTransformComponent>().connect(this, &TransformSystem::updateWorldTransform);
    scene.onRemoveComponent<WorldTransformComponent>().connect(this, &TransformSystem::removeWorldTransform);

    scene.onAddComponent<ParentComponent>().connect(this, &TransformSystem::addChild);
    scene.onUpdateComponent<ParentComponent>().connect(this, &TransformSystem::updateChild);
    scene.onRemoveComponent<ParentComponent>().connect(this, &TransformSystem::removeChild);
}

void 
TransformSystem::onLoad() {}

void 
TransformSystem::update(uint32_t) {

    // Since the system owns the source of truth of the hierarchy
    // the events are disconected, because it is always aware of changes
    // and also to prevent event loops and unnecesary event dispatching
    eventsDisconnected = true;

    std::vector<size_t> dirtyRoots = std::vector<size_t>();
    dirtyRoots.reserve(dense.size());

    for (EntityID dirtyEntity : dirtySet) {
        size_t dirtyNodeIndex = sparse[dirtyEntity];
        if (!hasDirtyAncestor(dirtyNodeIndex)) {
            dirtyRoots.push_back(dirtyNodeIndex);
        }
    }

    dirtyRoots.shrink_to_fit();

    for (size_t root : dirtyRoots) {
        calculateWorldTransform(root);
    }

    dirtySet.clear();

    // VERY IMPORTANT
    // we need to reconnect all the previous events that were disconnected
    eventsDisconnected = false;
}

void 
TransformSystem::fixedUpdate(uint32_t) {}

bool
TransformSystem::isLoop(size_t parentIndex, size_t childIndex) {

    // To prevent a stack overflow
    std::vector<size_t> stack;
    stack.reserve(dense.size());
    stack.push_back(parentIndex);

    size_t stackStart = 0;

    while(stackStart < stack.size()) {

        parentIndex = stack[stackStart];
        
        if (parentIndex == INVALID_NODE)
            return false;

        if (parentIndex == childIndex)
            return true;

        stack.push_back(dense[parentIndex].parent);

        ++stackStart;
    }

    return false;
}

void 
TransformSystem::calculateWorldTransform(size_t rootIndex) {

    // To prevent a stack overflow creates BFS search 
    // instead of DFS that recursion would create it still works fine
    std::vector<size_t> stack;
    stack.reserve(dense.size());
    stack.push_back(rootIndex);

    size_t stackStart = 0;

    while(stackStart < stack.size()) {
        
        TreeNode& node = dense[stack[stackStart]];

        const TransformComponent& localTransform =
            scene.getComponentConst<TransformComponent>(node.entity);

        // If no parent then local and world are the same
        if (node.parent == INVALID_NODE) {
            node.worldPosition = localTransform.position;
            node.worldRotation = localTransform.rotation;
            node.worldScale = localTransform.scale;
        }
        // If parent, get world of parent (it is ensured that it is calculated)
        else {

            TreeNode& parent = dense[node.parent];

            node.worldPosition = parent.worldPosition + parent.worldRotation * (
                localTransform.position * parent.worldScale
            );
            node.worldRotation = parent.worldRotation * localTransform.rotation;
            node.worldScale = parent.worldScale * localTransform.scale;
        }

        if (!scene.hasComponent<WorldTransformComponent>(node.entity)) {
            scene.addComponent<WorldTransformComponent>(node.entity);
        }

        // This modification will not reset the world component or change local since 
        // the events should be disconnected
        scene.modifyComponent<WorldTransformComponent>(node.entity, 
            [&node](WorldTransformComponent& worldTr){
                worldTr.worldPosition = node.worldPosition;
                worldTr.worldRotation = node.worldRotation;
                worldTr.worldScale = node.worldScale;
            }
        );

        node.dirty = false;

        for (size_t i = node.firstChild; i != INVALID_NODE; i = dense[i].nextSibling) {
            // Recurse down to children to update whole subtree
            stack.push_back(i);
        }

        ++stackStart;
    }
}

bool 
TransformSystem::hasDirtyAncestor(size_t potentialDirtyRoot) {

    size_t parentIndex = dense[potentialDirtyRoot].parent;

    if (parentIndex == INVALID_NODE)
        return false;

    // Here I would use a stack, but I wanna kill myself
    if (dense[parentIndex].dirty || hasDirtyAncestor(parentIndex)) {
        dense[potentialDirtyRoot].dirty = true;
        return true;
    } 

    return false;
}

void 
TransformSystem::markDirty(size_t dirtyIndex) {
    dirtySet.insert(dense[dirtyIndex].entity);
    dense[dirtyIndex].dirty = true;
}

void
TransformSystem::removeFromParent(size_t childIndex) {

    TreeNode& node = dense[childIndex];

    size_t parent = node.parent;

    if (parent == INVALID_NODE) return;

    if (dense[parent].firstChild == childIndex) {
        dense[parent].firstChild = node.nextSibling;
    }
    else {
        if (node.previousSibling != INVALID_NODE) {
            dense[node.previousSibling].nextSibling = node.nextSibling;
        }
    }

    if (node.nextSibling != INVALID_NODE) {
        dense[node.nextSibling].previousSibling = node.previousSibling;
    }

    node.parent = INVALID_NODE;
    node.nextSibling = INVALID_NODE;
    node.previousSibling = INVALID_NODE;
}

void
TransformSystem::attachNode(size_t parentIndex, size_t childIndex) {

    TreeNode& childNode  = dense[childIndex];

    if (childNode.parent == parentIndex)
        return;

    TreeNode& parentNode = dense[parentIndex];

    // detach from parent if any
    removeFromParent(childIndex);

    // If loop then stays detached, because it was going to be any ways
    if (isLoop(parentIndex, childIndex)) return;

    // link sibling list
    childNode.nextSibling = parentNode.firstChild;
    childNode.previousSibling = INVALID_NODE;

    if (parentNode.firstChild != INVALID_NODE)
    {
        dense[parentNode.firstChild].previousSibling = childIndex;
    }

    parentNode.firstChild = childIndex;

    childNode.parent = parentIndex;

    markDirty(childIndex);

    if (scene.hasComponent<WorldTransformComponent>(childNode.entity)) {
        worldToLocal(scene.getComponentConst<WorldTransformComponent>(childNode.entity));
    }
}

void
TransformSystem::detachNode(size_t detachedNode) {
    removeFromParent(detachedNode);
    markDirty(detachedNode);

    if (scene.hasComponent<WorldTransformComponent>(dense[detachedNode].entity)) {
        worldToLocal(scene.getComponentConst<WorldTransformComponent>(dense[detachedNode].entity));
    }
}

void
TransformSystem::swapAndDelete(size_t deleted) {

    size_t swapIndex = dense.size() - 1;
    
    EntityID swappedEntity = dense[swapIndex].entity;
    EntityID deletedEntity = dense[deleted].entity;

    dirtySet.erase(deletedEntity);
    sparse.erase(deletedEntity);

    if (deleted != swapIndex) {
        dense[deleted] = std::move(dense.back());
        sparse[swappedEntity] = deleted;

        TreeNode& swaped = dense[deleted];

        if (swaped.parent != INVALID_NODE) {
            if (dense[swaped.parent].firstChild == swapIndex) {
                dense[swaped.parent].firstChild = deleted;
            }
            else {
                if (swaped.previousSibling != INVALID_NODE) {
                    dense[swaped.previousSibling].nextSibling = deleted;
                }
            }
            if (swaped.nextSibling != INVALID_NODE) {
                dense[swaped.nextSibling].previousSibling = deleted;
            }
        }
        for (size_t i = swaped.firstChild; i != INVALID_NODE; i = dense[i].nextSibling) {
            
            dense[i].parent = deleted;
        }
    }

    dense.pop_back();
}

void 
TransformSystem::worldToLocal(const WorldTransformComponent& worldTrCmp) {

    eventsDisconnected = true;

    markDirty(sparse[worldTrCmp.entity]);

    TreeNode& node = dense[sparse[worldTrCmp.entity]];

    if (node.parent == INVALID_NODE) {
        scene.modifyComponent<TransformComponent>(worldTrCmp.entity, 
            [&worldTrCmp](TransformComponent& trCmp){
                trCmp.position = worldTrCmp.worldPosition;
                trCmp.rotation = worldTrCmp.worldRotation;
                trCmp.scale = worldTrCmp.worldScale;
            }
        );
    }
    else {

        TreeNode& parentNode = dense[node.parent];

        // Calculate new local from parent, this way local stays as updated 
        // source of truth for sync
        scene.modifyComponent<TransformComponent>(worldTrCmp.entity, 
            [&parentNode, &worldTrCmp](TransformComponent& localTransform){

                localTransform.position = parentNode.worldRotation.inverse() * 
                    (worldTrCmp.worldPosition - parentNode.worldPosition) / 
                    parentNode.worldScale;

                localTransform.rotation = parentNode.worldRotation.inverse() * 
                    worldTrCmp.worldRotation;

                localTransform.scale = worldTrCmp.worldScale / parentNode.worldScale;
            }
        );
    }

    eventsDisconnected = false;
}

void 
TransformSystem::localToWorld(const TransformComponent& trCmp) {

    eventsDisconnected = true;

    markDirty(sparse[trCmp.entity]);

    TreeNode& node = dense[sparse[trCmp.entity]];

    // If no parent then local and world are the same
    if (node.parent == INVALID_NODE) {
        scene.modifyComponent<WorldTransformComponent>(trCmp.entity, 
            [&trCmp](WorldTransformComponent& worldTrCmp){
                worldTrCmp.worldPosition = trCmp.position;
                worldTrCmp.worldRotation = trCmp.rotation;
                worldTrCmp.worldScale = trCmp.scale;
            }
        );
    }
    // If parent, get world of parent (it is ensured that it is calculated)
    else {
        TreeNode& parent = dense[node.parent];

        scene.modifyComponent<WorldTransformComponent>(trCmp.entity, 
            [&trCmp, &parent](WorldTransformComponent& worldTrCmp){
                worldTrCmp.worldPosition = parent.worldPosition + parent.worldRotation * (
                    trCmp.position * parent.worldScale);
                worldTrCmp.worldRotation = parent.worldRotation * trCmp.rotation;
                worldTrCmp.worldScale = parent.worldScale * trCmp.scale;
            }
        );
    }

    eventsDisconnected = false;
}


void
TransformSystem::initNode(TransformComponent& trCmp) {

    EntityID ent = trCmp.entity;

    if (sparse.count(ent)) return;

    // If there is a world, respect it
    if (scene.hasComponent<WorldTransformComponent>(trCmp.entity)) { 

        const WorldTransformComponent& worldTrCmp = 
            scene.getComponentConst<WorldTransformComponent>(trCmp.entity);

        trCmp.position = worldTrCmp.worldPosition;
        trCmp.rotation = worldTrCmp.worldRotation;
        trCmp.scale = worldTrCmp.worldScale;
    }

    dense.push_back({
        ent,
        trCmp.position,
        trCmp.rotation,
        trCmp.scale
    });

    sparse[ent] = dense.size() - 1;

    markDirty(dense.size() - 1);
}

void
TransformSystem::destroyNode(const TransformComponent& trCmp) { 
    
    size_t deletedIndex = sparse[trCmp.entity];

    TreeNode& node = dense[deletedIndex];
    
    for (size_t i = node.firstChild; i != INVALID_NODE;) {

        size_t next = dense[i].nextSibling;

        dense[i].parent = INVALID_NODE;
        dense[i].previousSibling = INVALID_NODE;
        dense[i].nextSibling = INVALID_NODE;

        markDirty(i);

        i = next;
    }

    removeFromParent(deletedIndex);

    swapAndDelete(deletedIndex);

    if (scene.hasComponent<ParentComponent>(trCmp.entity)) {
        scene.modifyComponent<ParentComponent>(trCmp.entity, 
            [](ParentComponent& childCmp){
                childCmp.parent = HUGO::INVALID_ENTITY;
            }
        );
    }
}

//--------------Transform events

void
TransformSystem::addTransform(TransformComponent& trCmp) {

    if (eventsDisconnected) return;

    if (!scene.hasComponent<WorldTransformComponent>(trCmp.entity)) {

        eventsDisconnected = true;
        scene.addComponent<WorldTransformComponent>(trCmp.entity);
        eventsDisconnected = false;
    }

    initNode(trCmp);

    localToWorld(trCmp);
}

void
TransformSystem::updateTransform(TransformComponent& trCmp) {

    if (eventsDisconnected) return;

    localToWorld(trCmp);
}

void
TransformSystem::removeTransform(TransformComponent& trCmp) {

    if (eventsDisconnected) return;

    destroyNode(trCmp);

    if (scene.hasComponent<WorldTransformComponent>(trCmp.entity)) {
        eventsDisconnected = true;
        scene.removeComponent<WorldTransformComponent>(trCmp.entity);
        eventsDisconnected = false;
    }
}

//--------------WorldTransform events

void
TransformSystem::addWorldTransform(WorldTransformComponent& worldTrCmp) { 

    if (eventsDisconnected) return;
    
    if (!scene.hasComponent<TransformComponent>(worldTrCmp.entity)) {
        eventsDisconnected = true;
        scene.addComponent<TransformComponent>(worldTrCmp.entity, [this](TransformComponent& trCmp){
            initNode(trCmp);
        });
        eventsDisconnected = false;
    }

    updateWorldTransform(worldTrCmp);
}

void
TransformSystem::updateWorldTransform(WorldTransformComponent& worldTrCmp) {

    if (eventsDisconnected) return;

    worldToLocal(worldTrCmp);
}

void
TransformSystem::removeWorldTransform(WorldTransformComponent& worldTrCmp) {

    if (eventsDisconnected) return;

    if (scene.hasComponent<TransformComponent>(worldTrCmp.entity)) {
        eventsDisconnected = true;
        destroyNode(scene.getComponentConst<TransformComponent>(worldTrCmp.entity)); 
        scene.removeComponent<TransformComponent>(worldTrCmp.entity);
        eventsDisconnected = false;
    }

}

//--------------Child events

void
TransformSystem::addChild(ParentComponent& childCmp) {

    if (eventsDisconnected) return;

    EntityID childEntity = childCmp.entity;
    EntityID parentEntity = childCmp.parent;

    if (!sparse.count(childEntity) || !sparse.count(parentEntity)) {
        childCmp.parent = HUGO::INVALID_ENTITY;
        return;
    }

    size_t parentNodeIndex = sparse[parentEntity];
    size_t childNodeIndex = sparse[childEntity];

    attachNode(parentNodeIndex, childNodeIndex);
}

void
TransformSystem::updateChild(ParentComponent& childCmp) {

    if (eventsDisconnected) return;

    EntityID childEntity = childCmp.entity;
    EntityID parentEntity = childCmp.parent;

    if (!sparse.count(childEntity) || !sparse.count(parentEntity)) {
        childCmp.parent = HUGO::INVALID_ENTITY;
        return;
    }

    size_t childNodeIndex = sparse[childEntity];
    TreeNode& child = dense[childNodeIndex];

    size_t parentNodeIndex = (parentEntity != HUGO::INVALID_ENTITY) ? 
        sparse[parentEntity] : INVALID_NODE;

    if (child.parent == parentNodeIndex) return; 

    if (parentNodeIndex != INVALID_NODE)
        attachNode(parentNodeIndex, childNodeIndex);
        if (child.parent != parentNodeIndex) {
            // Could not attach, set as no node
            childCmp.parent = HUGO::INVALID_ENTITY;
        }
    else
        detachNode(childNodeIndex);
}

void
TransformSystem::removeChild(ParentComponent& childCmp) {

    if (eventsDisconnected) return;

    if (!sparse.count(childCmp.entity)) return;

    size_t detached = sparse[childCmp.entity];

    detachNode(detached);
}