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

    std::unordered_map<std::string, AnimationInfo> anims;
    
    inline void addAnimation(const std::string& animationName, bool loop = false, bool enable = false, float weight = 1.0)
    {
        anims.insert({animationName, {loop, enable, weight}}); 
    }
    
    inline void removeAnimation(const std::string& animationName)
    {
        anims.erase(animationName);
    };

    inline void setLoop(const std::string& animationName, bool loop)
    {
        anims[animationName].loop = loop;
    };
    
    inline void setEnabled(const std::string& animationName, bool enable)
    {
        anims[animationName].enable = enable;
    };

    inline void setWeight(const std::string& animationName, float weight)
    {
        anims[animationName].weight = weight;
    };

END_HCOMPONENT()

