#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include <optional>
HCOMPONENT(TextComponent)
    DEFAULT_SERIALIZED_MEMBER(std::string, textAreaName, "");      // text area element name

    // text content
    DEFAULT_SERIALIZED_MEMBER(std::string, text, "");              // text to display
    DEFAULT_SERIALIZED_MEMBER(std::string, textFont, "Roboto");              // font name
    DEFAULT_SERIALIZED_MEMBER(float, characterHeight, 0.04f); // height in relative units
    DEFAULT_SERIALIZED_MEMBER(float, spaceWidth, 0.025f); // width in relative units

    // appearance
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colour, HUGO::Vector4(-1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colourTop, HUGO::Vector4(-1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colourBottom, HUGO::Vector4(-1.0f));

    DEFAULT_SERIALIZED_MEMBER(float, left, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, top, 0.0f);

    DEFAULT_SERIALIZED_MEMBER(float, maxWidth, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, maxHeight, 0.0f);
END_HCOMPONENT()

