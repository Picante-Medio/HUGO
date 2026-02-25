#pragma once
#include <limits>

namespace HUGO{
    using AudioID = int;
    using SceneID = int;

    using OgreObjectID = int;
    constexpr OgreObjectID INVALID_OGRE_OBJECT_ID = std::numeric_limits<int>::max();
    
    typedef enum { SOUND2D, SOUND3D, MUSIC, LAST_TYPE } AudioTypes;
}