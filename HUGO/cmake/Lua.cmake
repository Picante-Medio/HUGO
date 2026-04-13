include(FetchContent)
set(FETCHCONTENT_QUIET ON)

message(STATUS "========lua========")

FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/walterschell/Lua.git"
    GIT_TAG "v5.4.5"
    GIT_SHALLOW TRUE
    SYSTEM
)

set(LUA_ENABLE_TESTING OFF CACHE BOOL "")
set(LUA_BUILD_BINARY OFF CACHE BOOL "")
set(LUA_BUILD_COMPILER OFF CACHE BOOL "")
# Setting this option that the top level cmakelists of the lua dep defines makes it compile and link correctly
set(LUA_BUILD_AS_CXX ON CACHE BOOL "") 

FetchContent_MakeAvailable(lua)