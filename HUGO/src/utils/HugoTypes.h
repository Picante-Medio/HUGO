#pragma once
#include <functional>
#include <limits>
#include <string>

namespace HUGO{
    using OgreObjectID = int;
    constexpr OgreObjectID INVALID_OGRE_OBJECT_ID = std::numeric_limits<int>::max();

    using String = std::string;

    // Audio Types
    using AudioID = int;
    using SceneID = int;

    enum AudioTypes
    {
        SOUND2D,
        SOUND3D,
        MUSIC,
        LAST_TYPE
    };

    enum AudioDirtyFlags
    {
        LOOP = 0,
        VOLUME = 1 << 0,
        PITCH = 1 << 1,
        STARTTIME = 1 << 2,
        NONE_2D = 1 << 3
    };

    enum Audio3DDirtyFlags
    {
        MINDISTANCE = 0,
        MAXDISTANCE = 1 << 0,
        VELOCITY = 1 << 1,
        DIRECTION = 1 << 2,
        DOPPLER = 1 << 3,
        ROLLOF = 1 << 4,
        CONE = 1 << 5,
        MOVED = 1 << 6,
        NONE_3D = 1 << 7
    };

    using AudioEndCallback = std::function<void(HUGO::AudioID)>;

    // Render Types

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

    // -------- Collider Types --------
    enum class ColliderType 
    {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        CONE
    };

    enum BodyType{
        DYNAMIC,
        STATIC,
        KINEMATIC
    };
}
