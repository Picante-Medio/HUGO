#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(TextComponent)
    HUGO::String textAreaName = "textArea";     // text area element name

    // text content
    HUGO::String text = "";              // text to display
    HUGO::String textFont = "Roboto";              // font name
    HUGO::Float characterHeight = 0.04f; // height in relative units
    HUGO::Float spaceWidth = 0.025f; // width in relative units

    // appearance
    HUGO::Vector4 colour = HUGO::Vector4(-1.0f);
    HUGO::Vector4 colourTop = HUGO::Vector4(-1.0f); 
    HUGO::Vector4 colourBottom = HUGO::Vector4(-1.0f);
   
    HUGO::Float left = 0.0f;
    HUGO::Float top = 0.0f;
END_HCOMPONENT()

