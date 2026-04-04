#pragma once
#include <unordered_map>
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

struct AnimationInfo{
    bool loop = false;
    bool enable = false;
    float weight = 1.0;
};
HCOMPONENT(AnimationComponent)
    std::unordered_map<HUGO::String, AnimationInfo> anims;
    
    inline void addAnimation(const HUGO::String& animationName, bool loop = false, bool enable = false, float weight = 1.0)
    {
        anims.insert({animationName, {loop, enable, weight}}); 
    }
    
    inline void removeAnimation(const HUGO::String& animationName)
    {
        anims.erase(animationName);
    };

    inline void setLoop(const HUGO::String& animationName, bool loop)
    {
        anims[animationName].loop = loop;
    };
    
    inline void setEnabled(const HUGO::String& animationName, bool enable)
    {
        anims[animationName].enable = enable;
    };

    inline void setWeight(const HUGO::String& animationName, float weight)
    {
        anims[animationName].weight = weight;
    };

END_HCOMPONENT()

