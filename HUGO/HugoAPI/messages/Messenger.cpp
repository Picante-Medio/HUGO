#include "Messenger.h"

#include "Message.h"
#include "Channel.h"

namespace HUGO {

    struct Messenger::Impl {
        std::array<Channel, MAX_MESSAGES> channels;
        std::unordered_set<MsgID> activeChannels;
    };

    Messenger::Messenger() {
        pImpl = std::make_unique<Messenger::Impl>();
    }

    Messenger::~Messenger() = default;

    void 
    Messenger::sendAll() {
        for(MsgID id : pImpl->activeChannels) 
            pImpl->channels[id].sendMessages();

        pImpl->activeChannels.clear();
    }

    void
    Messenger::internalEnqueue(MsgID msgID, std::shared_ptr<Message> m) {
        pImpl->channels[msgID].enqueueMessage(m);
        pImpl->activeChannels.insert(msgID);
    }

    void
    Messenger::internalSubscribe(MsgID msgID, std::function<void(std::shared_ptr<Message>)> callback) {
        pImpl->channels[msgID].subscribeCallback(callback);
    }
};
