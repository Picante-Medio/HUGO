#pragma once
#include "HUGO.h"

#include <vector>

#include <limits>

struct ParentComponent;
struct TransformComponent;
struct WorldTransformComponent;

#include <unordered_set>

HSYSTEM(TransformSystem)

    struct TransformModifications;
    struct TreeNode;

    // Leveled modifications, consumed in update (sync phase)
    std::unordered_set<EntityID> dirtySet;

    // EntityID to node index, used for consult 
    std::unordered_map<EntityID, size_t> sparse = 
        std::unordered_map<EntityID, size_t>();
    // Dense tree nodes, hierarchy exist here
    std::vector<TreeNode> dense = std::vector<TreeNode>();

    bool hasDirtyAncestor(size_t potentialDirtyRoot);

    void markDirty(size_t dirtyIndex);

    // Swaps with last and then deletes last
    void swapAndDelete(size_t deleted);
    void removeFromParent(size_t node);

    void detachNode(size_t detachedNode);
    void attachNode(size_t parentIndex, size_t childIndex);

    bool isLoop(size_t parentIndex, size_t childIndex);

    void worldToLocal(const WorldTransformComponent& worldTrCmp);
    void localToWorld(const TransformComponent& localToWorld);

    void initNode(TransformComponent& trCmp);
    void destroyNode(const TransformComponent& trCmp);

    void addTransform(TransformComponent& trCmp);
    void updateTransform(TransformComponent& trCmp);
    void removeTransform(TransformComponent& trCmp);

    // Add and update is the same in case of world transform
    void addWorldTransform(WorldTransformComponent& worldTrCmp);
    void updateWorldTransform(WorldTransformComponent& worldTrCmp);
    void removeWorldTransform(WorldTransformComponent& trCmp);

    void addChild(ParentComponent& childCmp);
    void updateChild(ParentComponent& childCmp);
    void removeChild(ParentComponent& childCmp);

    void calculateWorldTransform(size_t rootIndex);

    bool eventsDisconnected = false;

public:
    void init() override;
    void onLoad() override;
    void update(uint32_t dt) override;
    void fixedUpdate(uint32_t) override;

END_HSYSTEM()
