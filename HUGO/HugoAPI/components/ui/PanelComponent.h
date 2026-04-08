#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(PanelComponent)
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, overlayName, "");
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, containerName, "");

    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, isVisible, true);

    DEFAULT_SERIALIZED_MEMBER(HUGO::UIMetricsMode, metricsMode, HUGO::UIMetricsMode::GMM_RELATIVE);

    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, left, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, top, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, width, 0.1f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, height, 0.1f);

    // HUGO::UIHorizontalAlignment alignmentX =
    //     HUGO::UIHorizontalAlignment::UHA_LEFT;

    // HUGO::UIVerticalAlignment alignmentY =
    //     HUGO::UIVerticalAlignment::UVA_CENTER;
        
    // appearance
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, material, "");
END_HCOMPONENT()

