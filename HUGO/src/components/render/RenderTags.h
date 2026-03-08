#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(CameraDirtyTag)
END_HCOMPONENT()

HCOMPONENT(ViewportDirtyTag)
END_HCOMPONENT()

HCOMPONENT(LightDirtyTag)
END_HCOMPONENT()

HCOMPONENT(MaterialDirtyTag)

    HUGO::String materialName;
    MaterialDirtyTag(EntityID entity, const HUGO::String& materialName) 
        : Component(entity), materialName(materialName) {}

END_HCOMPONENT()
