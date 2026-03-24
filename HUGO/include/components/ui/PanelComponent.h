#pragma once
#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(PanelComponent)
    HUGO::String overlayName = "";
    HUGO::String containerName = "";

    bool isVisible = true;

    // HUGO::UIMetricsMode metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;

    HUGO::Float left = 0.0f;
    HUGO::Float top = 0.0f;
    HUGO::Float width = 0.1f;
    HUGO::Float height = 0.1f;

    // HUGO::UIHorizontalAlignment alignmentX =
    //     HUGO::UIHorizontalAlignment::UHA_LEFT;

    // HUGO::UIVerticalAlignment alignmentY =
    //     HUGO::UIVerticalAlignment::UVA_CENTER;
        
    // appearance
    HUGO::String material = "";
END_HCOMPONENT()