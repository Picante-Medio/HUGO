#include "GameLibraryLoader.h"

HUGO::GameLibraryLoader::
GameLibraryLoader(std::string gameLibraryName) {
    gameHandle = LOAD_DYNAMIC_LIBRARY(gameLibraryName);
}


HUGO::GameLibraryLoader::
~GameLibraryLoader() {
    CLOSE_DYNAMIC_LIBRARY(gameHandle);
}

