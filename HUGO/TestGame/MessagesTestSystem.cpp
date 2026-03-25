#include "MessagesTestSystem.h"
#include "TestMessage.h"

void MessagesTestSystem::init() {
    // scene->subscribe<TestMessage>([](std::shared_ptr<HUGO::Message> msg){
    //     std::shared_ptr<TestMessage> m = std::static_pointer_cast<TestMessage>(msg);
    //     std::cout << m->s << '\n';
    // });
}

void MessagesTestSystem::onLoad() {

}
void MessagesTestSystem::fixedUpdate(double dt) {

}
void MessagesTestSystem::update(double dt) {
    currT += dt;
    if(currT >= nextT ) {
        // scene->sendMessage(TestMessage());

        currT = 0.f;
        nextT = 3.0f;
    }
}