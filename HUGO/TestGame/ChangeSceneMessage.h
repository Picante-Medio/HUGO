#include "HugoTypes.h"
#include "Message.h"
struct ChangeSceneMessage : HUGO::Message {
    HUGO::String sceneName = "TestScene";
};
