#pragma once
#include <functional>
#include <limits>
#include <string>

namespace HUGO{

    using Int = int32_t;
    
	constexpr float PI = 3.14159265358979323846f;

    // AUDIO TYPES
    using AudioID = int;
    using SceneID = int;

    enum AudioTypes
    {
        SOUND2D,
        SOUND3D,
        MUSIC,
        LAST_TYPE
    };


    using AudioEndCallback = std::function<void(HUGO::AudioID)>;

    // RENDER TYPES

    // -------- Camera Types --------

    // Polygon rendering modes
    enum PolygonMode
    {
        SOLID,
        WIREFRAME,
        POINTS
    };

    // Projection types for the camera
    enum ProjectionType
    {
        Perspective,
        Orthographic
    };

    // -------- Light Types --------
    enum LightType
    {
        LT_POINT,
        LT_DIRECTIONAL,
        LT_SPOTLIGHT,
        LT_RECTLIGHT
    };
    // -------- Button State Types --------
    enum ButtonStateTypes   /// enumerator values for button states
    {
        BS_UP,
        BS_OVER,
        BS_DOWN
    };

    // Relation to the parent in the horizontal dimension
    enum UIHorizontalAlignment
    {
        UHA_LEFT,
        UHA_CENTER,
        UHA_RIGHT
    };

    // Relation to the parent in the vertical dimension
    enum UIVerticalAlignment
    {
        UVA_TOP,
        UVA_CENTER,
        UVA_BOTTOM
    };
    
    enum UIMetricsMode
    {
        // Left, top, height and width are parametrics from 0.0 to 1.0
        GMM_RELATIVE,
        // Positions and sizes are in absolute pixels
        GMM_PIXELS,
        // Positions and sizes are in virtual pixels
        GMM_RELATIVE_ASPECT_ADJUSTED
    };

    enum ProgressBarFillMode { // enum for fill mode of the progress bars
        HORIZONTAL_LEFT_TO_RIGHT,
        HORIZONTAL_RIGHT_TO_LEFT,
        VERTICAL_BOTTOM_TO_TOP, 
        VERTICAL_TOP_TO_BOTTOM,
    };
    
    enum SliderOrientation {
        HORIZONTAL,
        VERTICAL
    };

    enum TrayLocationTypes  // enumerator values for widget tray anchoring locations
    {
        TL_TOPLEFT,
        TL_TOP,
        TL_TOPRIGHT,
        TL_LEFT,
        TL_CENTER,
        TL_RIGHT,
        TL_BOTTOMLEFT,
        TL_BOTTOM,
        TL_BOTTOMRIGHT,
        TL_NONE
    };
    
    // -------- PHYSICS TYPES --------

    using CollisionObjectID = int;
    constexpr CollisionObjectID INVALID_COL_OBJ_ID = std::numeric_limits<int>::max();

    using CollisionShapeID = int;
    constexpr CollisionShapeID INVALID_SHAPE_ID = std::numeric_limits<int>::max();

    constexpr int INVALID_INDEX = std::numeric_limits<int>::max();

}
