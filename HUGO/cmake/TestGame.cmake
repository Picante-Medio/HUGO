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
    HugoAPI)

add_custom_target(copy_config ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/TestGame/hugo.config
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/hugo.config
    COMMAND_EXPAND_LISTS)

if(MSVC)
    set_target_properties(
        TestGame copy_config
        PROPERTIES FOLDER "HUGO_engine/TestGame"
    )
endif()

# Visualizar los headers en el explorador de soluciones de visual studio
set(TestGame_HEADER_FILES)

file(GLOB_RECURSE HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/TestGame/*.h"
    "${CMAKE_SOURCE_DIR}/TestGame/*.hpp"
)
list(APPEND TestGame_HEADER_FILES ${HEADERS})

target_sources(TestGame PRIVATE
    ${TestGame_HEADER_FILES}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/TestGame"
    FILES
        ${TestGame_SRC_FILES}
        ${TestGame_HEADER_FILES}
)