add_library(LuaBinder STATIC ${CMAKE_SOURCE_DIR}/LuaBinder/LuaBinder.cpp)

enable_warnings(LuaBinder)

set_target_properties(LuaBinder PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    DEBUG_POSTFIX "d"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(LuaBinder PUBLIC ${CMAKE_SOURCE_DIR}/LuaBinder)
target_link_libraries(LuaBinder PUBLIC 
    HugoUtils
    lua_shared)