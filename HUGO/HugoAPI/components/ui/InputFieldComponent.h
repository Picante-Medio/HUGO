#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(InputFieldComponent)

    // indicates whether this input field is currently focused and receiving input
    DEFAULT_SERIALIZED_MEMBER(bool, isActive, false);

    // full text entered by the user (source for the input)
    HUGO::String fullText; 

    // original text before entering input mode (used to restore UI after submit/cancel)
    SERIALIZED_MEMBER(HUGO::String, originalText); 

    //text that appears when the input field is disabled
    SERIALIZED_MEMBER(HUGO::String, deactiveText); 

    // current cursor position within fullText
    size_t cursorPos = 0; 

    // horizontal scroll offset
    HUGO::Float scrollOffset = 0.0f;  

    // final result after submitting the input
    HUGO::String result = "";     

    // callback triggered when the input is submitted/changed
    std::function<void(const HUGO::String&)> onChangeCallback;

END_HCOMPONENT()

