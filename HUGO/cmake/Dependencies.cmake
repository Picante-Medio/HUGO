message(STATUS "================================")
message(STATUS "Configuring HUGO dependencies...")
message(STATUS "================================")

include(FetchContent)
set(FETCHCONTENT_QUIET ON)

message(STATUS "======pugixml======")

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.14
    OVERRIDE_FIND_PACKAGE
    SYSTEM
)

FetchContent_MakeAvailable(pugixml)

message(STATUS "=====miniaudio=====")

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY "https://github.com/mackron/miniaudio.git"
    GIT_TAG "0.11.24"
    GIT_SHALLOW TRUE
    SYSTEM
)

set(MINIAUDIO_NO_EXTRA_NODES ON CACHE BOOL "")
set(MINIAUDIO_FORCE_CXX ON CACHE BOOL "")
set(MINIAUDIO_INSTALL OFF CACHE BOOL "")
set(MINIAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "")

FetchContent_MakeAvailable(miniaudio)

message(STATUS "======Bullet3======")

FetchContent_Declare(
    bullet3
    GIT_REPOSITORY "https://github.com/bulletphysics/bullet3.git"
    GIT_TAG 3.25
    GIT_SHALLOW TRUE
    SYSTEM
)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "")
set(BUILD_PYBULLET OFF CACHE BOOL "")
set(BUILD_EXTRAS OFF CACHE BOOL "")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "")
set(INSTALL_LIBS OFF CACHE BOOL "")
set(INSTALL_CMAKE_FILES OFF CACHE BOOL "")
set(BUILD_CLSOCKET OFF CACHE BOOL "")
set(BUILD_ENET OFF CACHE BOOL "")
set(ENABLE_VHACD OFF CACHE BOOL "")
set(USE_GLUT OFF CACHE BOOL "")
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "")
set(USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD OFF CACHE BOOL "")
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "")

FetchContent_MakeAvailable(bullet3)

message(STATUS "======assimp=======")

FetchContent_Declare(
    assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp.git"
    GIT_TAG "v6.0.4"
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE
    SYSTEM
)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "") # This is not default in linux, so we set it to ensure linkage is PIC
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_OGRE_IMPORTER OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_MINIZIP ON CACHE BOOL "")
FetchContent_MakeAvailable(assimp)
# makes it visible for Ogre to compile the Codec_Assimp plugin target
get_target_property(_assimp_includes assimp INTERFACE_INCLUDE_DIRECTORIES)
set(ASSIMP_INCLUDE_DIRS ${_assimp_includes})
set(ASSIMP_LIBRARIES assimp::assimp)
set(ASSIMP_LIBRARY_DIRS "")

if(WIN32)

    message(STATUS "=======SDL2========")

    FetchContent_Declare(
        sdl2
        GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
        GIT_TAG "release-2.30.7"
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE 
        SYSTEM
    )

    set(SDL_SHARED ON CACHE BOOL "")
    set(SDL_STATIC OFF CACHE BOOL "")
    set(SDL2_DISABLE_INSTALL ON CACHE BOOL "")
    set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "")
    set(SDL_TEST OFF CACHE BOOL "")
    set(SDL_AUDIO OFF CACHE BOOL "")
    set(SDL_VIDEO ON CACHE BOOL "")
    set(SDL_RENDER OFF CACHE BOOL "")
    set(SDL_JOYSTICK OFF CACHE BOOL "")
    set(SDL_HAPTIC OFF CACHE BOOL "")
    set(SDL_HIDAPI OFF CACHE BOOL "")
    set(SDL_POWER OFF CACHE BOOL "")
    set(SDL_FILE OFF CACHE BOOL "")
    set(SDL_LOADSO ON CACHE BOOL "")
    set(SDL_CPUINFO OFF CACHE BOOL "")
    set(SDL_FILESYSTEM OFF CACHE BOOL "")
    set(SDL_SENSOR OFF CACHE BOOL "")
    set(SDL_LOCALE OFF CACHE BOOL "")
    set(SDL_MISC OFF CACHE BOOL "")

    FetchContent_MakeAvailable(sdl2)

elseif(LINUX)
    find_package(SDL2 CONFIG REQUIRED)
endif()

# later, after the other deps
message(STATUS "======OGRE3D=======")

FetchContent_Declare(
    ogre
    GIT_REPOSITORY "https://github.com/OGRECave/ogre.git"
    GIT_TAG "v14.5.2"
    GIT_SHALLOW TRUE
    OVERRIDE_FIND_PACKAGE 
    SYSTEM
)

# Enabled options
set(OGRE_BITES_NATIVE_DIALOG OFF CACHE BOOL "")
set(OGRE_BITES_STATIC_PLUGINS OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_BITES OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_BULLET OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_MESHLODGENERATOR OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_OVERLAY_IMGUI OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_PAGING OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_PROPERTY OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_TERRAIN OFF CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_VOLUME OFF CACHE BOOL "")
set(OGRE_BUILD_DEPENDENCIES OFF CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_BSP OFF CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_DOT_SCENE OFF CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_GLSLANG OFF CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_RSIMAGE OFF CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_D3D11 OFF CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_GLES2 OFF CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_D3D9 OFF CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_TINY OFF CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_VULKAN OFF CACHE BOOL "")
set(OGRE_BUILD_SAMPLES OFF CACHE BOOL "")
set(OGRE_BUILD_BUILD_TESTS OFF CACHE BOOL "")
set(OGRE_BUILD_BUILD_TOOLS OFF CACHE BOOL "")
set(OGRE_CONFIG_ENABLE_QUAD_BUFFER_STEREO OFF CACHE BOOL "")
set(OGRE_INSTALL_DEPENDENCIES OFF CACHE BOOL "")
set(OGRE_INSTALL_DOCS OFF CACHE BOOL "")
set(OGRE_INSTALL_SAMPLES OFF CACHE BOOL "")
set(OGRE_INSTALL_TOOLS OFF CACHE BOOL "")
set(OGRE_INSTALL_VSPROPS OFF CACHE BOOL "")
set(OGRE_NODELESS_POSITIONING OFF CACHE BOOL "")
set(OGRE_STATIC OFF CACHE BOOL "")

# Disabled options
set(OGRE_BUILD_COMPONENT_OVERLAY ON CACHE BOOL "")
set(OGRE_BUILD_COMPONENT_RTSHADERSYSTEM ON CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_OCTREE ON CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_PCZ ON CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_PFX ON CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_STBI ON CACHE BOOL "")
set(OGRE_BUILD_PLUGIN_ASSIMP ON CACHE BOOL "")
set(OGRE_BUILD_RENDERSYSTEM_GL3PLUS ON CACHE BOOL "")
set(OGRE_BUILD_RTSHADERSYSTEM_SHADERS ON CACHE BOOL "")
set(OGRE_CONFIG_FILESYSTEM_UNICODE ON CACHE BOOL "")
set(OGRE_COPY_DEPENDENCIES ON CACHE BOOL "")
set(OGRE_ENABLE_PRECOMPILED_HEADERS ON CACHE BOOL "")
set(OGRE_INSTALL_PDB ON CACHE BOOL "")
set(OGRE_PROJECT_FOLDERS ON CACHE BOOL "")

if(NOT WIN32) # Resets to default because ogre does not define the postfix for non windows systems
    set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "" FORCE)
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable(ogre)

set_target_properties(RenderSystem_GL PROPERTIES SYSTEM TRUE)

# defined target to link dependencies
add_library(HugoDeps INTERFACE)

# Patch to setup includes that do not export them in their targets
target_include_directories(HugoDeps SYSTEM INTERFACE
    ${bullet3_SOURCE_DIR}/src)

target_link_libraries(HugoDeps INTERFACE
    lua_shared
    BulletDynamics
    BulletCollision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Collision
    LinearMath
    OgreMain
    OgreOverlay
    SDL2::SDL2
    SDL2::SDL2main
    miniaudio)