#pragma once
#include "HugoComponentMacros.h"
#include "HugoTypes.h"

HCOMPONENT(InputFieldComponent)

    // indicates whether this input field is currently focused and receiving input
    DEFAULT_SERIALIZED_MEMBER(bool, isActive, false);

    // full text entered by the user (source for the input)
    std::string fullText; 

    // original text before entering input mode (used to restore UI after submit/cancel)
    SERIALIZED_MEMBER(std::string, originalText); 

    //text that appears when the input field is disabled
    SERIALIZED_MEMBER(std::string, deactiveText); 

    // current cursor position within fullText
    size_t cursorPos = 0; 

    // horizontal scroll offset
    float scrollOffset = 0.0f;  

    // final result after submitting the input
    std::string result = "";     

    // callback triggered when the input is submitted/changed
    std::function<void(const std::string&)> onChangeCallback;

END_HCOMPONENT()

