file(GLOB_RECURSE HugoAPI_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/HugoAPI/*)
set(HugoAPI_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/HugoAPI)

foreach(path ${HugoAPI_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND HugoAPI_INCLUDE_DIRS "${path}")
    endif()
endforeach()

add_library(HugoAPI INTERFACE)

set_target_properties(HugoAPI PROPERTIES
    DEBUG_POSTFIX "d"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(HugoAPI INTERFACE ${HugoAPI_INCLUDE_DIRS})

# Temporary
target_link_libraries(HugoAPI INTERFACE 
    SDL2::SDL2
    SDL2::SDL2main)