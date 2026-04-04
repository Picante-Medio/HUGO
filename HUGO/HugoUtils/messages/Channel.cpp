#include "Channel.h"
#include <memory>
#include <queue>
#include <utility>

HUGO::Channel::Channel() 
    : messages(), callbacks()
{}

void HUGO::Channel::sendMessages() {
    std::queue<std::shared_ptr<Message>> currentIterationMessages = std::move(messages);
    messages = std::queue<std::shared_ptr<Message>>();

    while(!currentIterationMessages.empty()) {
        std::shared_ptr<Message> msg = currentIterationMessages.front();

        for(auto callback : callbacks)
            callback(msg);

        currentIterationMessages.pop();
    }
}

void HUGO::Channel::subscribeCallback(std::function<void(std::shared_ptr<Message>)> callback) {
    callbacks.push_back(callback);
}

void HUGO::Channel::enqueueMessage(std::shared_ptr<Message> msg) {
    messages.push(msg);
}
