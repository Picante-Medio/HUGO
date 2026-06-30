file(GLOB_RECURSE HugoAPI_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_CURRENT_SOURCE_DIR}/HugoAPI/*)
set(HugoAPI_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/HugoAPI)

foreach(path ${HugoAPI_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND HugoAPI_INCLUDE_DIRS "${path}")
    endif()
endforeach()

file(GLOB_RECURSE HugoAPI_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/HugoAPI/*.cpp)

add_library(HugoAPI SHARED ${HugoAPI_SRC_FILES})
enable_warnings(HugoAPI)

set_target_properties(HugoAPI PROPERTIES
    DEBUG_POSTFIX "d"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
target_compile_definitions(HugoAPI PRIVATE BUILD_UTILS_API)

target_include_directories(HugoAPI PUBLIC ${HugoAPI_INCLUDE_DIRS})
target_link_libraries(HugoAPI PUBLIC 
    lua_shared)

get_target_property(HUGO_INCLUDES HugoAPI INCLUDE_DIRECTORIES)

if(WIN32)
    set_target_properties(
        HugoAPI
        PROPERTIES FOLDER "HUGO_engine"
    )
endif()

# Visualizar los headers en el explorador de soluciones de visual studio
set(HugoAPI_HEADER_FILES)

foreach(dir ${HugoAPI_INCLUDE_DIRS})
    file(GLOB_RECURSE HEADERS
        CONFIGURE_DEPENDS
        "${dir}/*.h"
        "${dir}/*.hpp"
    )
    list(APPEND HugoAPI_HEADER_FILES ${HEADERS})
endforeach()

target_sources(HugoAPI PRIVATE
    ${HugoAPI_HEADER_FILES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/HugoAPI"
    FILES
        ${HugoAPI_SRC_FILES}
        ${HugoAPI_HEADER_FILES}
)