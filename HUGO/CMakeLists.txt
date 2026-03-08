cmake_minimum_required(VERSION 3.30.0)
project(HUGO LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.24
)

FetchContent_Declare(
    bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 3.25
)

FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/walterschell/Lua.git"
    GIT_TAG "v5.4.5"
    GIT_SHALLOW TRUE
)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_FILES OFF CACHE BOOL "" FORCE)
# Lua 
set(LUA_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(LUA_BUILD_BINARY OFF CACHE BOOL "" FORCE)
set(LUA_BUILD_COMPILER OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(miniaudio bullet3 lua)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

file(GLOB_RECURSE SRC_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/src/*)

set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src
    ${bullet3_SOURCE_DIR}/src)

foreach(path ${SRC_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND INCLUDE_DIRS "${path}")
    endif()
endforeach()

list(APPEND INCLUDE_DIRS "${bullet3_SOURCE_DIR}/src")

file(GLOB_RECURSE BULLET_SRC_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${bullet3_SOURCE_DIR}/src/*)

foreach(path ${BULLET_SRC_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND INCLUDE_DIRS "${path}")
    endif()
endforeach()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/HugoCore)

set(BASE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/dependencies/${CMAKE_SYSTEM_PROCESSOR})

if(WIN32)
    set(CMAKE_PREFIX_PATH 
        ${BASE_PREFIX_PATH}/sdl2
        ${BASE_PREFIX_PATH}/ogre
        CACHE PATH "prefix path"
    )
elseif(LINUX)
    set(CMAKE_PREFIX_PATH
        ${BASE_PREFIX_PATH}/ogre
        CACHE PATH "prefix path")

    set(CMAKE_EXE_LINKER_FLAGS "-lpng -lbz2 -lbrotlidec -lpthread -lm")
    
    # find_package(Threads REQUIRED)
    # find_library(BROTLIDEC_LIB brotlidec REQUIRED)
    # find_library(PNG_LIB png REQUIRED)
    # find_library(BZ2_LIB bz2 REQUIRED)
    # find_library(M_LIB m REQUIRED)

    # list(APPEND HUGO_LINKED_LIBRARIES
    #     Threads::Threads
    #     ${BROTLIDEC_LIB}
    #     ${PNG_LIB}
    #     ${BZ2_LIB}
    #     ${M_LIB})
endif()

find_package(SDL2 CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED COMPONENTS
    Overlay)

set(HUGO_LINKED_LIBRARIES
    BulletDynamics
    BulletCollision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Collision
    LinearMath
    OgreMain
    OgreOverlay
    RenderSystem_GL3Plus
    RenderSystem_GL
    SDL2::SDL2
    SDL2::SDL2main
    miniaudio)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${HUGO_LINKED_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif ()
