#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(TextComponent)
    HUGO::String textAreaName = "textArea";     // text area element name

    // text content
    HUGO::String text = "";              // text to display
    HUGO::Float characterHeight = 0.05f; // height in relative units

    // appearance
    HUGO::Vector4 colour = HUGO::Vector4(-1.0f);
    HUGO::Vector4 colourTop = HUGO::Vector4(-1.0f); 
    HUGO::Vector4 colourBottom = HUGO::Vector4(-1.0f);
        
    // space width
    HUGO::Float spaceWidth = 0.03f;  
END_HCOMPONENT()