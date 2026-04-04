#pragma once

#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(TransformComponent)
private:
    //world transform
    HUGO::Vector3 worldPosition = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion worldRotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 worldScale = HUGO::Vector3(1, 1, 1);

    //Local transform
    HUGO::Vector3 position = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion rotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 scale = HUGO::Vector3(1, 1, 1);

    bool needUpdateLocal = false;
    bool needUpdateWorld = false;
public:
    inline void setPosition(HUGO::Vector3 newPosition){
        position = newPosition;
        needUpdateWorld = true;
    }
    inline void setRotation(HUGO::Quaternion newRotation){
        rotation = newRotation;
        needUpdateWorld = true;
    }
    inline void setScale(HUGO::Vector3 newScale){
        scale = newScale;
        needUpdateWorld = true;
    }
    inline HUGO::Vector3 getPosition() const{
        return position;
    }
    inline HUGO::Quaternion getRotation() const{
        return rotation;
    }
    inline HUGO::Vector3 getScale() const{
        return scale;
    }

    inline void setWorldPosition(HUGO::Vector3 newPosition){
        worldPosition = newPosition;
        needUpdateLocal = true;
    }
    inline void setWorldRotation(HUGO::Quaternion newRotation){
        worldRotation = newRotation;
        needUpdateLocal = true;
    }
    inline void setWorldScale(HUGO::Vector3 newScale){
        worldScale = newScale;
        needUpdateLocal = true;
    }
    inline const HUGO::Vector3 getWorldPosition() const{
        return worldPosition;
    }
    inline const HUGO::Quaternion getWorldRotation() const{
        return worldRotation;
    }
    inline const HUGO::Vector3 getWorldScale() const{
        return worldScale;
    }

    friend class HierarchySystem;
END_HCOMPONENT()

HCOMPONENT(TransformDirtyTag)
END_HCOMPONENT()

