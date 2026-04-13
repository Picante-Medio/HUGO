#include "HugoTypes.h"
#include "Message.h"
struct ChangeSceneMessage : HUGO::Message {
    HUGO::String sceneName = "TestScene";
};

struct ChangeSceneMessageVideo1 : HUGO::Message {
    HUGO::String sceneName = "TestSceneVideo2";
};

struct ChangeSceneMessageVideo2 : HUGO::Message {
    HUGO::String sceneName = "TestSceneVideo3";
};

struct ChangeSceneMessageVideo3 : HUGO::Message {
    HUGO::String sceneName = "TestSceneVideo1";
};