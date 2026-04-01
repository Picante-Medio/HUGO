#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(CameraDirtyTag)
END_HCOMPONENT()

HCOMPONENT(ViewportDirtyTag)
END_HCOMPONENT()

HCOMPONENT(LightDirtyTag)
END_HCOMPONENT()

HCOMPONENT(CameraCreateDirtyTag)
END_HCOMPONENT()

HCOMPONENT(ViewportCreateDirtyTag)
END_HCOMPONENT()

HCOMPONENT(LightCreateDirtyTag)
END_HCOMPONENT()


HCOMPONENT(MaterialDirtyTag)
    HUGO::String materialName = "";
END_HCOMPONENT()

