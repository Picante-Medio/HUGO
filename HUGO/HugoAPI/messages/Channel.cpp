#include "Channel.h"
#include <memory>
#include <queue>
#include <utility>
#include <iostream>

namespace HUGO {

    struct Channel::Impl {
        std::queue<std::shared_ptr<Message>> messages;
        std::vector<std::function<void(std::shared_ptr<Message>)>> callbacks;

    };

    Channel::Channel() 
    : pImpl(std::make_unique<Channel::Impl>())
    {
    }

    Channel::~Channel() = default;

    void HUGO::Channel::sendMessages() {
        std::queue<std::shared_ptr<Message>> currentIterationMessages = std::move(pImpl->messages);
        pImpl->messages = std::queue<std::shared_ptr<Message>>();

        while(!currentIterationMessages.empty()) {
            std::shared_ptr<Message> msg = currentIterationMessages.front();

            for(auto callback : pImpl->callbacks)
                callback(msg);

            currentIterationMessages.pop();
        }
    }

    void HUGO::Channel::subscribeCallback(std::function<void(std::shared_ptr<Message>)> callback) {
        pImpl->callbacks.push_back(callback);
    }

    void HUGO::Channel::enqueueMessage(std::shared_ptr<Message> msg) {
        pImpl->messages.push(msg);
    }

}