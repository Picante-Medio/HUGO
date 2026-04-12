#include "HugoTypes.h"
#include "Message.h"
struct ChangeSceneMessage : HUGO::Message {
    HUGO::String sceneName = "TestScene";
};

struct ChangeSceneMessageVideo1 : HUGO::Message {
    HUGO::String sceneName = "TestSceneVideo2";
};
