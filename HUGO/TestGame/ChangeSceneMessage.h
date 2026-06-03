#pragma once
#include "HugoTypes.h"
#include "Message.h"
struct ChangeSceneMessage : HUGO::Message {
    std::string sceneName = "TestScene";
};

struct ChangeSceneMessageVideo1 : HUGO::Message {
    std::string sceneName = "TestSceneVideo2";
};

struct ChangeSceneMessageVideo2 : HUGO::Message {
    std::string sceneName = "TestSceneVideo3";
};

struct ChangeSceneMessageVideo3 : HUGO::Message {
    std::string sceneName = "TestSceneVideo1";
};