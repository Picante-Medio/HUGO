#pragma once
#include <functional>
#include <limits>
#include <string>

namespace HUGO{
    using OgreObjectID = int;
    constexpr OgreObjectID INVALID_OGRE_OBJECT_ID = std::numeric_limits<int>::max();

    using AudioID = int;
    using SceneID = int;

    using String = std::string;

    enum AudioTypes
    {
        SOUND2D,
        SOUND3D,
        MUSIC,
        LAST_TYPE
    };

    enum AudioDirtyFlags
    {
        NONE = 0,
        LOOP = 1 << 0,
        VOLUME = 1 << 1,
        PITCH = 1 << 2,
        STARTTIME = 1 << 3,
        START = 1 << 4,
        STOP = 1 << 5
    };

    enum Audio3DDirtyFlags
    {
        NONE_3D = 0,
        MINDISTANCE = 1 << 0,
        MAXDISTANCE = 1 << 1,
        VELOCITY = 1 << 2,
        DIRECTION = 1 << 3,
        DOPPLER = 1 << 4,
        ROLLOF = 1 << 5,
        CONE = 1 << 6,
        MOVED = 1 << 7
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
    

    // -------- PHYSICS TYPES --------

    using RigidBodyID = int;
    constexpr RigidBodyID INVALID_RB_ID = std::numeric_limits<int>::max();

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
