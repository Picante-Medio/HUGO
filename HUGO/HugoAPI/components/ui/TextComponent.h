#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(TextComponent)
    HUGO::String textAreaName = "textArea";     // text area element name

    // text content
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, text, "");              // text to display
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, textFont, "Roboto");              // font name
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, characterHeight, 0.04f); // height in relative units
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, spaceWidth, 0.025f); // width in relative units

    // appearance
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colour, HUGO::Vector4(-1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colourTop, HUGO::Vector4(-1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colourBottom, HUGO::Vector4(-1.0f));

    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, left, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, top, 0.0f);
END_HCOMPONENT()

