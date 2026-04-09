#pragma once

#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(TransformComponent)
private:
    //Local transform
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, position, HUGO::Vector3())
    DEFAULT_SERIALIZED_MEMBER(HUGO::Quaternion, rotation, HUGO::Quaternion())
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, scale, HUGO::Vector3())
    
    //world transform
    HUGO::Vector3 worldPosition = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion worldRotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 worldScale = HUGO::Vector3(1, 1, 1);

    bool needUpdateLocalPosition = false;
    bool needUpdateWorldPosition = false;
    bool needUpdateLocalRotation = false;
    bool needUpdateWorldRotation = false;
    bool needUpdateLocalScale = false;
    bool needUpdateWorldScale = false;

public:
    inline void setPosition(HUGO::Vector3 newPosition){
        position = newPosition;
        needUpdateWorldPosition = true;
    }
    inline void setRotation(HUGO::Quaternion newRotation){
        rotation = newRotation;
        needUpdateWorldRotation= true;
    }
    inline void setScale(HUGO::Vector3 newScale){
        scale = newScale;
        needUpdateWorldScale = true;
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
        needUpdateLocalPosition = true;
    }
    inline void setWorldRotation(HUGO::Quaternion newRotation){
        worldRotation = newRotation;
        needUpdateLocalRotation = true;
    }
    inline void setWorldScale(HUGO::Vector3 newScale){
        worldScale = newScale;
        needUpdateLocalScale = true;
    }
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

