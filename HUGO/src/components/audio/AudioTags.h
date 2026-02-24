#pragma once
#include "Component.h"

struct ReproduceAudioTag: public Component{
    ReproduceAudioTag(EntityID entity): Component(entity) {}
};
