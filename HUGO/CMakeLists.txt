cmake_minimum_required(VERSION 3.30.0)
project(HUGO LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
set(CMAKE_WARN_DEPRECATED OFF)
set(POSITION_INDEPENDENT_CODE ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Functions.cmake)

################################## HugoAPI

# Can be overrided
set(HUGO_API_LIB_DIR "${CMAKE_SOURCE_DIR}/lib" CACHE STRING "Binary directory for HUGO library files")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Lua.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/HugoAPI.cmake)

################################## HUGO executable engine

option(HUGO_BUILD_ENGINE "Builds the HUGO executable" ON)

if(HUGO_BUILD_ENGINE)

    # Can be overrided
    set(HUGO_BIN_DIR "${CMAKE_SOURCE_DIR}/bin" CACHE STRING "Binary directory for HUGO executable")

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

    file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

    add_executable(${PROJECT_NAME} ${SRC_FILES})
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

    enable_warnings(${PROJECT_NAME})

    file(GLOB_RECURSE SRC_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/src/*)

    set(INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

    foreach(path ${SRC_DIRS})
        if(IS_DIRECTORY "${path}")
            list(APPEND INCLUDE_DIRS "${path}")
        endif()
    endforeach()

    target_include_directories(${PROJECT_NAME} PRIVATE 
        ${INCLUDE_DIRS})

    target_link_libraries(${PROJECT_NAME} PRIVATE 
        HugoDeps
        HugoAPI)

    set(CMAKE_SKIP_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

    if (WIN32) 
        set_target_properties(
                ${PROJECT_NAME}
                PROPERTIES FOLDER "HUGO_engine"
        )
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        DEBUG_POSTFIX "d"
        RUNTIME_OUTPUT_DIRECTORY ${HUGO_BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${HUGO_BIN_DIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${HUGO_BIN_DIR}
        VS_DEBUGGER_WORKING_DIRECTORY ${HUGO_BIN_DIR}
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
        WIN32_EXECUTABLE $<$<CONFIG:Release>:TRUE>
    )


    # Ogre plugins are independent, so they need to be explicitly copied in both platforms
    foreach(plugin RenderSystem_GL3Plus RenderSystem_GL Codec_STBI Codec_Assimp Plugin_ParticleFX OgreRTShaderSystem)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${plugin}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS)
    endforeach()

    if (WIN32)
        add_custom_command(TARGET ${PROJECT_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
        )
    else()
        # Copy generated .so libs to the bin dir
        foreach(lib lua_shared OgreMain OgreOverlay)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${lib}>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS)
        endforeach()
    endif ()
    
endif()

################Test Game########################

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/TestGame.cmake)
endif()