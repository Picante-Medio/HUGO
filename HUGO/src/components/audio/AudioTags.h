#pragma once

#include "Component.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioDirtyTag)

    HUGO::AudioDirtyFlags flags = HUGO::AudioDirtyFlags::NONE;

    AudioDirtyTag(EntityID entity,
                  HUGO::AudioDirtyFlags flags)
        : Component(entity), flags(flags) {}

END_HCOMPONENT()

HCOMPONENT(Audio3DDirtyTag)

    HUGO::Audio3DDirtyFlags flags = HUGO::Audio3DDirtyFlags::NONE_3D;

    Audio3DDirtyTag(EntityID entity,
                  HUGO::Audio3DDirtyFlags flags)
        : Component(entity), flags(flags) {}

END_HCOMPONENT()

HCOMPONENT(AudioPlayTag)
END_HCOMPONENT()

HCOMPONENT(AudioStopTag)
END_HCOMPONENT()

HCOMPONENT(AudioResumeTag)
END_HCOMPONENT()