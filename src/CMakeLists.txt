cmake_minimum_required(VERSION 3.30.0)
project(HUGO LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED COMPONENTS
    Overlay)

file(GLOB_RECURSE SRC_DIRS CONFIGURE_DEPENDS  LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/*)

set(INCLUDE_DIRS "${CMAKE_SOURCE_DIR}")
foreach(path ${SRC_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND INCLUDE_DIRS "${path}")
    endif()
endforeach()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    OgreMain
    OgreOverlay
    RenderSystem_GL3Plus
    RenderSystem_GL
    SDL2::SDL2
    SDL2::SDL2main)

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY ${ENGINE_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_BIN_DIR})

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
endif ()