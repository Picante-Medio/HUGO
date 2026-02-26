#pragma once
#include <functional>
#include <limits>
namespace HUGO{
    using OgreObjectID = int;
    constexpr OgreObjectID INVALID_OGRE_OBJECT_ID = std::numeric_limits<int>::max();

    using AudioID = int;
    using SceneID = int;
    
    typedef enum { SOUND2D, SOUND3D, MUSIC, LAST_TYPE } AudioTypes;
    typedef enum {
    NONE   = 0,
    LOOP   = 1 << 0,
    VOLUME = 1 << 1,
    PITCH  = 1 << 2,
    STARTTIME  = 1 << 3,
    START = 1 << 4,
    STOP = 1 << 5
    } AudioDirtyFlags;

    typedef enum {
    NONE_3D   = 0,
    MINDISTANCE = 1 << 0,
    MAXDISTANCE = 1 << 1,
    VELOCITY  = 1 << 2,
    DIRECTION  = 1 << 3,
    DOPPLER = 1 << 4,
    ROLLOF = 1 << 5,
    CONE = 1 << 6,
    MOVED = 1 << 7
    } Audio3DDirtyFlags;
    using AudioEndCallback = std::function<void(HUGO::AudioID)>;
}