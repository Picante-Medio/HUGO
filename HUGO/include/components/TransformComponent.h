#pragma once

#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(TransformComponent)
private:
    //world transform
    HUGO::Vector3 worldPosition = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion worldRotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 worldScale = HUGO::Vector3(1, 1, 1);
public:
    //Local transform
    HUGO::Vector3 position = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion rotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 scale = HUGO::Vector3(1, 1, 1);

    inline HUGO::Vector3 getWorldPosition() const{
        return worldPosition;
    }
    inline HUGO::Quaternion getWorldRotation() const{
        return worldRotation;
    }
    inline HUGO::Vector3 getWorldScale() const{
        return worldScale;
    }

    friend class HierarchySystem;
END_HCOMPONENT()

HCOMPONENT(TransformDirtyTag)
END_HCOMPONENT()

