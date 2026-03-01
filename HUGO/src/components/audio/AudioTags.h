#pragma once

#include "Component.h"
#include "HugoTypes.h"

struct AudioDirtyTag : public Component
{
    HUGO::AudioDirtyFlags flags;

    AudioDirtyTag(EntityID entity,
                  HUGO::AudioDirtyFlags flags = HUGO::AudioDirtyFlags::NONE)
        : Component(entity), flags(flags) {}
};

struct Audio3DDirtyTag : public Component
{
    HUGO::Audio3DDirtyFlags flags;

    Audio3DDirtyTag(EntityID entity,
                  HUGO::Audio3DDirtyFlags flags = HUGO::Audio3DDirtyFlags::NONE_3D)
        : Component(entity), flags(flags) {}
};

struct AudioPlayTag: public Component{
    AudioPlayTag(EntityID entity)
            : Component(entity){}
};

struct AudioStopTag: public Component{
    AudioStopTag(EntityID entity)
            : Component(entity){}
};