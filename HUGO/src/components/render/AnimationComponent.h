#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(AnimationComponent)
private:
    HUGO::String currentAnim = "";
    bool isLoop;
    bool enabled;
public:
    // Setter
    void setAnimation(const HUGO::String& AnimName) { currentAnim = AnimName; }
    void setLoop(bool loop) {isLoop=loop;}
    void setEnabled(bool enable) { enabled = enable;}
    // Getters
    HUGO::String getAnimation() const { return currentAnim; }
    bool getEnabled() const {return enabled;}
    bool getLoop() const {return isLoop;}
END_HCOMPONENT()

