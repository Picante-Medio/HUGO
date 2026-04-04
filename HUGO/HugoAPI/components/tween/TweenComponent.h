#pragma once
#include "HugoComponentMacros.h"
#include "HugoTween.h"
#include <memory>
#include <list>

//To tween a parameter, append a HUGO::Tween::makeTween(...) to currentTweens
HCOMPONENT(TweenComponent)

    std::list<std::shared_ptr<HUGO::ITween>> currentTweens;

END_HCOMPONENT()

