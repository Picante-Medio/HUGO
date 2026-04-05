cmake_minimum_required(VERSION 3.30.0)
project(HUGO LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

set(FETCHCONTENT_QUIET ON)
set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
set(CMAKE_WARN_DEPRECATED OFF)

include(cmake/Dependencies.cmake)
include(cmake/Functions.cmake)

################################## HugoLib

include(cmake/HugoAPI.cmake)
include(cmake/HugoUtils.cmake)
include(cmake/LuaBinder.cmake)

################################## Aquí empieza el proyecto principal

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_FILES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

enable_warnings(${PROJECT_NAME})

file(GLOB_RECURSE SRC_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/src/*)

set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)

foreach(path ${SRC_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND INCLUDE_DIRS "${path}")
    endif()
endforeach()

set(SYSTEM_INCLUDE_DIRS ${HUGO_DEPS_INCLUDE_DIRS})  # ya contiene bullet3

foreach(lib IN LISTS HUGO_LINKED_LIBRARIES)
    if(TARGET ${lib})
        get_target_property(inc_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
        if(inc_dirs)
            list(APPEND SYSTEM_INCLUDE_DIRS ${inc_dirs})
        endif()
    endif()
endforeach()

list(REMOVE_DUPLICATES SYSTEM_INCLUDE_DIRS)


target_include_directories(${PROJECT_NAME} PRIVATE 
    ${INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
    ${SYSTEM_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${HUGO_LINKED_LIBRARIES}
    HugoAPI
    HugoUtils
    LuaBinder)

set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    INSTALL_RPATH "$ORIGIN"
    BUILD_WITH_INSTALL_RPATH TRUE)

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    )
else()
    foreach(lib lua_shared OgreMain OgreOverlay RenderSystem_GL3Plus RenderSystem_GL Codec_STBI)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${lib}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS)
    endforeach()
endif ()

########################################

include(cmake/TestGame.cmake)