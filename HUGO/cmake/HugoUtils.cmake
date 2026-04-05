file(GLOB_RECURSE HugoUtils_DIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/HugoUtils/*)
set(HugoUtils_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/HugoUtils)

foreach(path ${HugoUtils_DIRS})
    if(IS_DIRECTORY "${path}")
        list(APPEND HugoUtils_INCLUDE_DIRS "${path}")
    endif()
endforeach()

file(GLOB_RECURSE HugoUtils_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/HugoUtils/*.cpp)

add_library(HugoUtils STATIC ${HugoUtils_SRC_FILES})
enable_warnings(HugoUtils)

set_target_properties(HugoUtils PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    DEBUG_POSTFIX "d"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(HugoUtils PUBLIC ${HugoUtils_INCLUDE_DIRS})
target_link_libraries(HugoUtils PUBLIC HugoAPI)