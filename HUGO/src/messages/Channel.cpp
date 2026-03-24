#include "Channel.h"

HUGO::Channel::Channel() 
    : messages(), callbacks()
{}

void HUGO::Channel::sendMessages() {
    while(!messages.empty()) {
        std::shared_ptr<Message> msg = std::make_shared<Message>(messages.front());

        for(auto callback : callbacks)
            callback(msg);

        messages.pop();
    }
}

void HUGO::Channel::subscribeCallback(std::function<void(std::shared_ptr<Message>)> callback) {
    callbacks.push_back(callback);
}
