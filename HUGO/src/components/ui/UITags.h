#pragma once

#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(VisibilityDirtyTag)
END_HCOMPONENT()

HCOMPONENT(PanelDirtyTag)
    //HUGO::UInt flags = HUGO::PanelDirtyTags::NONE; 
END_HCOMPONENT()

HCOMPONENT(TextDirtyTag) 
    //HUGO::UInt flags = HUGO::TextDirtyTags::NONE;  
END_HCOMPONENT() 

HCOMPONENT(ButtonDirtyTag) 
END_HCOMPONENT() 

HCOMPONENT(ProgressBarDirtyTag) 
    //HUGO::UInt flags = HUGO::ProgressBarDirtyTags::NONE;  
END_HCOMPONENT() 
