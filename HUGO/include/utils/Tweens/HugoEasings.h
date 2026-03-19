#pragma once

struct Easing{
    virtual float easingFunction(float t);
};

struct LinearTween: public Easing{
    inline float easingFunction(float t) override{
        return t;
    }
};

class EaseInBackTween: public Easing{
    inline float easingFunction(float t) override{
        float c1 = 1.70158f;
        float c3 = c1 +1;
        return c3 * t * t * t - c1 * t * t;
    }
};

class EaseInExponentialTween: public Easing{
    inline float easingFunction(float t) override{
        if(t ==0) return 0;
        return pow(2, 10 * t - 10);
    }
};

class EaseOutQuintTween: public Easing{
    inline float easingFunction(float t) override{
        return 1- pow(1-t, 5);
    }
};

class EaseInOutCubicTween: public Easing{
    inline float easingFunction(float t) override{
        if(t < 0.5) return 4 * t * t * t;
        return 1 - pow(-2 * t + 2, 3) / 2;
    }
    
};

class EaseOutElasticTween: public Easing{
    inline float easingFunction(float t) override{
        if(t ==0) return 0;
        else if (t ==1) return 1;

        float c4 = (2 * M_PI) / 3;
        return pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1;
    }
};