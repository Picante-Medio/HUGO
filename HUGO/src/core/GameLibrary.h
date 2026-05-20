#pragma once

#include <string>
#include <memory>

#include "HugoError.h"
#include "IScene.h"
#include "HugoParsingVariant.h"
#include "System.h"
#include "LuaBinder.h"

#include <memory>

namespace HUGO {

class GameLibrary final {

    struct Impl;
    Impl *pImpl;

public:

    GameLibrary(const GameLibrary&) = delete;
    GameLibrary& operator=(const GameLibrary&) = delete;
    GameLibrary(GameLibrary&&) = delete;

    GameLibrary();
    GameLibrary(const std::string& gameLibraryName);
    ~GameLibrary();

    GameLibrary& operator=(GameLibrary&&) = default;
};

}
