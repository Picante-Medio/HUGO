#pragma once
#include <unordered_map>
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

struct AnimationInfo{
    bool loop = false;
    bool enable = false;
};
HCOMPONENT(AnimationComponent)
    std::unordered_map<HUGO::String, AnimationInfo> anims;
    
    inline void addAnimation(const HUGO::String& animationName, bool loop = false, bool enable = false)
    {
        anims.insert({animationName, {loop, enable}}); 
    }
    
    inline void removeAnimation(const HUGO::String& animationName)
    {
        anims.erase(animationName);
    };

    inline void setLoop(const HUGO::String& animationName, bool loop = false)
    {
        anims[animationName].loop = loop;
    };
    
    inline void setEnabled(const HUGO::String& animationName, bool enable = false)
    {
        anims[animationName].enable = enable;
    };

END_HCOMPONENT()

