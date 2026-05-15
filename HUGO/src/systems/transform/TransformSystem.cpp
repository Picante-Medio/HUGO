#include "TransformSystem.h"

#include "ParentComponent.h"
#include "TransformComponent.h"
#include "WorldTransformComponent.h"

#include <algorithm>

static constexpr size_t INVALID_NODE = std::numeric_limits<size_t>::max();

struct TransformSystem::Node {

    EntityID entity;

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
    dirtyRoots.reserve(dirtyEntities.size());

    for (EntityID dirtyEntity : dirtyEntities) {
        size_t dirtyNodeIndex = entityToIndex[dirtyEntity];
        if (!hasDirtyAncestor(dirtyNodeIndex)) {
            dirtyRoots.push_back(dirtyNodeIndex);
        }
    }

    for (size_t root : dirtyRoots) {
        calculateWorldTransform(root);
    }

    dirtyEntities.clear();

    // VERY IMPORTANT
    // we need to reconnect all the previous events that were disconnected
    eventsDisconnected = false;
}

void 
TransformSystem::fixedUpdate(uint32_t) {}

size_t 
TransformSystem::getIndex(EntityID e) const {
    return entityToIndex.at(e);
}

bool
TransformSystem::isLoop(size_t parentIndex, size_t childIndex) {

    // To prevent a stack overflow
    std::vector<size_t> stack;
    stack.reserve(nodes.size());
    stack.push_back(parentIndex);

    size_t stackStart = 0;

    while(stackStart < stack.size()) {

        parentIndex = stack[stackStart];
        
        if (parentIndex == INVALID_NODE)
            return false;

        if (parentIndex == childIndex)
            return true;

        stack.push_back(nodes[parentIndex].parent);

        ++stackStart;
    }

    return false;
}

void 
TransformSystem::calculateWorldTransform(size_t rootIndex) {

    // To prevent a stack overflow creates BFS search 
    // instead of DFS that recursion would create it still works fine
    std::vector<size_t> stack;
    stack.reserve(nodes.size());
    stack.push_back(rootIndex);

    size_t stackStart = 0;

    while(stackStart < stack.size()) {
        
        Node& node = nodes[stack[stackStart]];

        const TransformComponent& localTransform =
            scene.getComponentConst<TransformComponent>(node.entity);

        // If no parent then local and world are the same
        if (node.parent == INVALID_NODE) {
            scene.modifyComponent<WorldTransformComponent>(node.entity, 
                [&localTransform](WorldTransformComponent& worldTr){
                    worldTr.worldPosition = localTransform.position;
                    worldTr.worldRotation = localTransform.rotation;
                    worldTr.worldScale = localTransform.scale;
                }
            );
        }
        // If parent, get world of parent (it is ensured that it is calculated)
        else {

            Node& parent = nodes[node.parent];
            const WorldTransformComponent& parentWTr = scene.getComponentConst<WorldTransformComponent>(parent.entity);

            // This modification will not reset the world component or change local since 
            // the events should be disconnected
            scene.modifyComponent<WorldTransformComponent>(node.entity, 
                [&localTransform, &parentWTr](WorldTransformComponent& worldTr){
                    worldTr.worldPosition = parentWTr.worldPosition + parentWTr.worldRotation * (
                        localTransform.position * parentWTr.worldScale
                    );
                    worldTr.worldRotation = parentWTr.worldRotation * localTransform.rotation;
                    worldTr.worldScale = parentWTr.worldScale * localTransform.scale;
                }
            );
        }

        if (!scene.hasComponent<WorldTransformComponent>(node.entity)) {
            scene.addComponent<WorldTransformComponent>(node.entity);
        }

        node.dirty = false;

        for (size_t i = node.firstChild; i != INVALID_NODE; i = nodes[i].nextSibling) {
            // Recurse down to children to update whole subtree
            stack.push_back(i);
        }

        ++stackStart;
    }
}

bool 
TransformSystem::hasDirtyAncestor(size_t idx) {


    size_t p = nodes[idx].parent;

    while (p != INVALID_NODE) {
        if (nodes[p].dirty) {
            nodes[idx].dirty = true;
            return true;
        }
        p = nodes[p].parent;
    }

    return false;
}

void 
TransformSystem::markDirty(size_t dirtyIndex) {
    dirtyEntities.insert(nodes[dirtyIndex].entity);
    nodes[dirtyIndex].dirty = true;
}

void
TransformSystem::removeFromParent(size_t childIndex) {

    Node& node = nodes[childIndex];

    size_t parent = node.parent;

    if (parent == INVALID_NODE) return;

    if (nodes[parent].firstChild == childIndex) {
        nodes[parent].firstChild = node.nextSibling;
    }
    else {
        if (node.previousSibling != INVALID_NODE) {
            nodes[node.previousSibling].nextSibling = node.nextSibling;
        }
    }

    if (node.nextSibling != INVALID_NODE) {
        nodes[node.nextSibling].previousSibling = node.previousSibling;
    }

    node.parent = INVALID_NODE;
    node.nextSibling = INVALID_NODE;
    node.previousSibling = INVALID_NODE;
}

void
TransformSystem::attachNode(size_t parentIndex, size_t childIndex) {

    Node& childNode  = nodes[childIndex];

    if (childNode.parent == parentIndex)
        return;

    Node& parentNode = nodes[parentIndex];

    // detach from parent if any
    removeFromParent(childIndex);

    // If loop then stays detached, because it was going to be any ways
    if (isLoop(parentIndex, childIndex)) return;

    // link sibling list
    childNode.nextSibling = parentNode.firstChild;
    childNode.previousSibling = INVALID_NODE;

    if (parentNode.firstChild != INVALID_NODE){
        nodes[parentNode.firstChild].previousSibling = childIndex;
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

    if (scene.hasComponent<WorldTransformComponent>(nodes[detachedNode].entity)) {
        worldToLocal(scene.getComponentConst<WorldTransformComponent>(nodes[detachedNode].entity));
    }
}

void
TransformSystem::swapAndDelete(size_t deleted) {

    size_t swapIndex = nodes.size() - 1;
    
    EntityID swappedEntity = nodes[swapIndex].entity;
    EntityID deletedEntity = nodes[deleted].entity;

    dirtyEntities.erase(deletedEntity);
    entityToIndex.erase(deletedEntity);

    if (deleted != swapIndex) {
        nodes[deleted] = std::move(nodes.back());
        entityToIndex[swappedEntity] = deleted;

        Node& swaped = nodes[deleted];

        if (swaped.parent != INVALID_NODE) {
            if (nodes[swaped.parent].firstChild == swapIndex) {
                nodes[swaped.parent].firstChild = deleted;
            }
            else {
                if (swaped.previousSibling != INVALID_NODE) {
                    nodes[swaped.previousSibling].nextSibling = deleted;
                }
            }
            if (swaped.nextSibling != INVALID_NODE) {
                nodes[swaped.nextSibling].previousSibling = deleted;
            }
        }
        for (size_t i = swaped.firstChild; i != INVALID_NODE; i = nodes[i].nextSibling) {
            
            nodes[i].parent = deleted;
        }
    }

    nodes.pop_back();
}

void 
TransformSystem::worldToLocal(const WorldTransformComponent& worldTrCmp) {

    eventsDisconnected = true;

    markDirty(entityToIndex[worldTrCmp.entity]);

    Node& node = nodes[entityToIndex[worldTrCmp.entity]];

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

        Node& parent = nodes[node.parent];

        const WorldTransformComponent& parentWTr = scene.getComponentConst<WorldTransformComponent>(parent.entity);

        // Calculate new local from parent, this way local stays as updated 
        // source of truth for sync
        scene.modifyComponent<TransformComponent>(worldTrCmp.entity, 
            [&parentWTr, &worldTrCmp](TransformComponent& localTransform){

                localTransform.position = parentWTr.worldRotation.inverse() * 
                    (worldTrCmp.worldPosition - parentWTr.worldPosition) / 
                    parentWTr.worldScale;

                localTransform.rotation = parentWTr.worldRotation.inverse() * 
                    worldTrCmp.worldRotation;

                localTransform.scale = worldTrCmp.worldScale / parentWTr.worldScale;
            }
        );
    }

    eventsDisconnected = false;
}

void 
TransformSystem::localToWorld(const TransformComponent& trCmp) {

    eventsDisconnected = true;

    markDirty(entityToIndex[trCmp.entity]);

    Node& node = nodes[entityToIndex[trCmp.entity]];

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
    else {
        Node& parent = nodes[node.parent];

        const WorldTransformComponent& parentWTr = scene.getComponentConst<WorldTransformComponent>(parent.entity);

        scene.modifyComponent<WorldTransformComponent>(trCmp.entity, 
            [&trCmp, &parentWTr](WorldTransformComponent& worldTrCmp){
                worldTrCmp.worldPosition = parentWTr.worldPosition + parentWTr.worldRotation * (
                    trCmp.position * parentWTr.worldScale);
                worldTrCmp.worldRotation = parentWTr.worldRotation * trCmp.rotation;
                worldTrCmp.worldScale = parentWTr.worldScale * trCmp.scale;
            }
        );
    }

    eventsDisconnected = false;
}


void
TransformSystem::initNode(TransformComponent& trCmp) {

    EntityID ent = trCmp.entity;

    if (entityToIndex.count(ent)) return;

    // If there is a world, respect it
    if (scene.hasComponent<WorldTransformComponent>(trCmp.entity)) { 

        const WorldTransformComponent& worldTrCmp = 
            scene.getComponentConst<WorldTransformComponent>(trCmp.entity);

        trCmp.position = worldTrCmp.worldPosition;
        trCmp.rotation = worldTrCmp.worldRotation;
        trCmp.scale = worldTrCmp.worldScale;
    }

    nodes.push_back({
        ent
    });

    entityToIndex[ent] = nodes.size() - 1;

    markDirty(nodes.size() - 1);
}

void
TransformSystem::destroyNode(const TransformComponent& trCmp) { 
    
    size_t deletedIndex = entityToIndex[trCmp.entity];

    Node& node = nodes[deletedIndex];
    
    for (size_t i = node.firstChild; i != INVALID_NODE;) {

        size_t next = nodes[i].nextSibling;

        nodes[i].parent = INVALID_NODE;
        nodes[i].previousSibling = INVALID_NODE;
        nodes[i].nextSibling = INVALID_NODE;

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

    initNode(trCmp);

    if (!scene.hasComponent<WorldTransformComponent>(trCmp.entity)) {
        eventsDisconnected = true;
        // Very important, if not all will be 0
        scene.addComponent<WorldTransformComponent>(trCmp.entity, [&trCmp](WorldTransformComponent& worldTr){
            worldTr.worldPosition = trCmp.position;
            worldTr.worldRotation = trCmp.rotation;
            worldTr.worldScale = trCmp.scale;
        });

        eventsDisconnected = false;
    }

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

    if (!entityToIndex.count(childEntity) || !entityToIndex.count(parentEntity)) {
        childCmp.parent = HUGO::INVALID_ENTITY;
        return;
    }

    size_t parentNodeIndex = entityToIndex[parentEntity];
    size_t childNodeIndex = entityToIndex[childEntity];

    attachNode(parentNodeIndex, childNodeIndex);
}

void
TransformSystem::updateChild(ParentComponent& childCmp) {

    if (eventsDisconnected) return;

    EntityID childEntity = childCmp.entity;
    EntityID parentEntity = childCmp.parent;

    if (!entityToIndex.count(childEntity) || !entityToIndex.count(parentEntity)) {
        childCmp.parent = HUGO::INVALID_ENTITY;
        return;
    }

    size_t childNodeIndex = entityToIndex[childEntity];

    size_t parentNodeIndex = (parentEntity != HUGO::INVALID_ENTITY) 
        ? entityToIndex[parentEntity] 
        : INVALID_NODE;

    if (nodes[childNodeIndex].parent == parentNodeIndex) return; 

    if (parentNodeIndex != INVALID_NODE) {
        attachNode(parentNodeIndex, childNodeIndex);
        if (nodes[childNodeIndex].parent != parentNodeIndex) {
            // Could not attach, set as no node
            childCmp.parent = HUGO::INVALID_ENTITY;
        }
    }
    else
        detachNode(childNodeIndex);
}

void
TransformSystem::removeChild(ParentComponent& childCmp) {

    if (eventsDisconnected) return;

    if (!entityToIndex.count(childCmp.entity)) return;

    size_t detached = entityToIndex[childCmp.entity];

    detachNode(detached);
}