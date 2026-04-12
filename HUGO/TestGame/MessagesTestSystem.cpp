#include "MessagesTestSystem.h"
#include "ChangeSceneMessage.h"
#include <memory>
#include <iostream>

void MessagesTestSystem::init() {
    scene.subscribe<ChangeSceneMessage>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessage> m = std::static_pointer_cast<ChangeSceneMessage>(msg);
        scene.changeAndDestroyPreviousScene(m->sceneName);
    });
}

void MessagesTestSystem::onLoad() {

}
void MessagesTestSystem::fixedUpdate(double dt) {

}
void MessagesTestSystem::update(double dt) {

    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::TWO)){
        std::cout << "Cambiando escena...\n";

        scene.sendMessage<ChangeSceneMessage>(ChangeSceneMessage{});
    }
}
