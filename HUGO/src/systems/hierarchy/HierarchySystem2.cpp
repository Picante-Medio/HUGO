#include "HierarchySystem2.h"

#include "EldestComponent.h"
#include "ChildrenComponent2.h"
#include "ParentComponent2.h"
#include "TransformComponent.h"

void 
HierarchySystem2::init() {
    scene.onAddComponent<ChildrenComponent2>().connect(this, &HierarchySystem2::onChildrenComponentAdded);
    scene.onAddComponent<ParentComponent2>().connect(this, &HierarchySystem2::onParentComponentAdded);

    scene.onUpdateComponent<ChildrenComponent2>().connect(this, &HierarchySystem2::onChildrenComponentUpdated);
    scene.onUpdateComponent<ParentComponent2>().connect(this, &HierarchySystem2::onParentComponentUpdated);
}

void 
HierarchySystem2::update(uint32_t) {
    for(auto pack : scene.query<EldestComponent, ChildrenComponent2, TransformComponent>()) {
        const ChildrenComponent2& children = pack.getConst<ChildrenComponent2>();
        const TransformComponent parentTr = pack.getConst<TransformComponent>();

        updateTransformRecursive(children, parentTr);
    }
}

void 
HierarchySystem2::updateTransformRecursive(const ChildrenComponent2& cc, const TransformComponent& parentTr) {
    for(EntityID child : cc.children) {
        scene.modifyComponent<TransformComponent>(child, [parentTr](TransformComponent& tr){
            tr.setWorldPosition(tr.getPosition() + parentTr.getWorldPosition());
        });

        if(scene.hasComponent<ChildrenComponent2>(child));
        const ChildrenComponent2& recursiveCC = scene.getComponentConst<ChildrenComponent2>(child);
        updateTransformRecursive(recursiveCC, scene.getComponentConst<TransformComponent>(child));
    }
}

void 
HierarchySystem2::onChildrenComponentAdded(ChildrenComponent2& cc) {
    if(!scene.hasComponent<ParentComponent2>(cc.entity)) {
        scene.addComponent<EldestComponent>(cc.entity);
    }

    for(EntityID child : cc.children) {
        //Add parent component if entity isn't parented
        if(!scene.hasComponent<ParentComponent2>(child)) {
           scene.addComponent<ParentComponent2>(child, [cc](ParentComponent2& parentComponent){
                parentComponent.setParent(cc.entity);
           });
        }

        // Change old parent for new parent
        else {
            scene.modifyComponent<ParentComponent2>(child, [this, cc](ParentComponent2& pc) {
                pc.setParent(cc.entity);

                scene.modifyComponent<ChildrenComponent2>(pc.getOldParent(), [cc](ChildrenComponent2& oldChildComp){
                    oldChildComp.removeChild(cc.entity);
                });
            });
        }
    }
}

void 
HierarchySystem2::onParentComponentAdded(ParentComponent2& pc) {
    if(scene.hasComponent<EldestComponent>(pc.entity)) {
        scene.removeComponent<EldestComponent>(pc.entity);
    }
}

void 
HierarchySystem2::onParentComponentUpdated(ParentComponent2& pc) {
    scene.modifyComponent<ChildrenComponent2>(pc.getOldParent(), [pc](ChildrenComponent2& cc){
        cc.removeChild(pc.entity);
    });
}

void 
HierarchySystem2::onChildrenComponentUpdated() {
    
}
