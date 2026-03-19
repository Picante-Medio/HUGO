#pragma once
#include "HugoComponentMacros.h"
#include "HugoTween.h"
#include <memory>

//To tween a parameter, append a HUGO::Tween to currentTweens
HCOMPONENT(TweenComponent)

    std::vector<std::shared_ptr<HUGO::ITween>> currentTweens;

    

END_HCOMPONENT()