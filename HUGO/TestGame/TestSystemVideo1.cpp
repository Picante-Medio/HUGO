#include "TestSystemVideo1.h"
#include "ChangeSceneMessage.h"


void
TestSystemVideo1::init() {
    scene.subscribe<ChangeSceneMessage>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessage> m = std::static_pointer_cast<ChangeSceneMessage>(msg);
        scene.changeAndDestroyPreviousScene(m->sceneName);
    });
}


void
TestSystemVideo1::onLoad() {}


void
TestSystemVideo1::update(double dt) {
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::THREE)){
        std::cout << "Cambiando escena...\n";
        ChangeSceneMessage c;
        c.sceneName = "TestSceneVideo3";
        scene.sendMessage<ChangeSceneMessage>(c);
    }
}

void
TestSystemVideo1::fixedUpdate(double) {}
