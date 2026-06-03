#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(PanelComponent)
    DEFAULT_SERIALIZED_MEMBER(std::string, overlayName, "");
    DEFAULT_SERIALIZED_MEMBER(std::string, containerName, "");

    DEFAULT_SERIALIZED_MEMBER(bool, isVisible, true);

    DEFAULT_SERIALIZED_MEMBER(HUGO::UIMetricsMode, metricsMode, HUGO::UIMetricsMode::GMM_RELATIVE);

    DEFAULT_SERIALIZED_MEMBER(float, left, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, top, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, width, 0.1f);
    DEFAULT_SERIALIZED_MEMBER(float, height, 0.1f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Int, zOrder, 100); 
    // HUGO::UIHorizontalAlignment alignmentX =
    //     HUGO::UIHorizontalAlignment::UHA_LEFT;

    // HUGO::UIVerticalAlignment alignmentY =
    //     HUGO::UIVerticalAlignment::UVA_CENTER;
        
    // appearance
    DEFAULT_SERIALIZED_MEMBER(std::string, material, "");
END_HCOMPONENT()

