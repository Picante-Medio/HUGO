#pragma once
#include <math.h>
#include "HugoMath.h"

struct Easing{
    virtual HUGO::Float easingFunction(HUGO::Float t) =0;
};

struct LinearTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        return t;
    }
};

class EaseInBackTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        HUGO::Float c1 = 1.70158f;
        HUGO::Float c3 = c1 +1;
        return c3 * t * t * t - c1 * t * t;
    }
};

class EaseInExponentialTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        if(t ==0) return 0;
        return pow(2, 10 * t - 10);
    }
};

class EaseOutQuintTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        return 1- pow(1-t, 5);
    }
};

class EaseInOutCubicTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        if(t < 0.5) return 4 * t * t * t;
        return 1 - pow(-2 * t + 2, 3) / 2;
    }
    
};

class EaseOutElasticTween: public Easing{
    inline HUGO::Float easingFunction(HUGO::Float t) override{
        if(t ==0) return 0;
        else if (t ==1) return 1;

        HUGO::Float c4 = (2 * M_PI) / 3;
        return pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1;
    }
};