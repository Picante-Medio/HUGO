#pragma once
#include <cmath>
#include "HugoTypes.h"

struct Easing{
    virtual float easingFunction(float t) = 0;
};

struct LinearTween: public Easing{
    inline float easingFunction(float t) override{
        return t;
    }
};

struct EaseInBackTween: public Easing{
    inline float easingFunction(float t) override{
        float c1 = 1.70158f;
        float c3 = c1 +1;
        return c3 * t * t * t - c1 * t * t;
    }
};

struct EaseInExponentialTween: public Easing{
    inline float easingFunction(float t) override{
        if(t ==0) return 0;
        return static_cast<float>(pow(2, 10 * t - 10));
    }
};

struct EaseOutQuintTween: public Easing{
    inline float easingFunction(float t) override{
        return static_cast<float>(1- pow(1-t, 5));
    }
};

struct EaseInOutCubicTween: public Easing{
    inline float easingFunction(float t) override{
        if(t < 0.5) return 4 * t * t * t;
        return static_cast<float>(1 - pow(-2 * t + 2, 3) / 2);
    }
    
};

struct EaseOutElasticTween: public Easing{
    inline float easingFunction(float t) override{
        const float c4 = (2.f * HUGO::PI) / 3.f;

        return static_cast<float>(t == 0
        ? 0
        : t == 1
        ? 1
        : pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1);
    }
};