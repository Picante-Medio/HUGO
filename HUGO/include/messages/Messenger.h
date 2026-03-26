#pragma once

#include <functional>
#include <memory>
#include <array>
#include <unordered_set>

#include "Channel.h"
#include "Message.h"

namespace HUGO {
class Messenger {
    std::array<Channel, MAX_MESSAGES> channels;
    std::unordered_set<MsgID> activeChannels;

public:
    Messenger();

    template<typename TMessage>
    void subscribe(std::function<void(std::shared_ptr<Message>)> callback) {
        MsgID msgID = getMsgID<TMessage>();
        channels[msgID].subscribeCallback(callback);
    }

    template<typename TMessage>
    void enqueue(const TMessage& msg) {
        MsgID msgID = getMsgID<TMessage>();
        std::shared_ptr<Message> m = std::static_pointer_cast<Message>(std::make_shared<TMessage>(msg));
        channels[msgID].enqueueMessage(m);
        activeChannels.insert(msgID);
    }

    void sendAll();
};
}
