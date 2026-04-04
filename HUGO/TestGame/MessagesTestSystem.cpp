#include "MessagesTestSystem.h"
#include "ChangeSceneMessage.h"
#include <memory>

void MessagesTestSystem::init() {
    // scene->subscribe<ChangeSceneMessage>([this](std::shared_ptr<HUGO::Message> msg) {
    //     std::shared_ptr<ChangeSceneMessage> m = std::static_pointer_cast<ChangeSceneMessage>(msg);
    //     scene->changeScene(m->sceneName);
    // });
}

void MessagesTestSystem::onLoad() {

}
void MessagesTestSystem::fixedUpdate(double dt) {

}
void MessagesTestSystem::update(double dt) {
    currT += dt;
    if(currT >= nextT ) {
        scene->sendMessage<ChangeSceneMessage>(ChangeSceneMessage{});
    }
}
