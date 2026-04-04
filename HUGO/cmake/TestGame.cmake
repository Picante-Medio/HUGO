################## Juego de prueba

file(GLOB_RECURSE TestGame_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/TestGame/*.cpp)

add_library(TestGame MODULE ${TestGame_SRC_FILES})
enable_warnings(TestGame)

set_target_properties(TestGame PROPERTIES
    DEBUG_POSTFIX "d"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

target_include_directories(TestGame PRIVATE ${CMAKE_SOURCE_DIR}/TestGame)

target_link_libraries(TestGame PRIVATE 
    HugoAPI
    HugoUtils
    LuaBinder)
