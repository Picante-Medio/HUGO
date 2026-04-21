#pragma once
#pragma warning(push)
#pragma warning(disable:4251) // this warning is a false positive

#include "UtilsAPI.h"

#include <functional>
#include <memory>
#include <array>
#include <unordered_set>

#include "Message.h"

class Channel;

namespace HUGO {

    class UTILS_API Messenger {

        // pImpl idiom to hide implementation details
        struct Impl;
        std::unique_ptr<Impl> pImpl; // Ignore warning, Impl is an incomplete type here. Its full definition is in the implementation file

        void internalEnqueue(MsgID msgID, std::shared_ptr<Message> m);
        void internalSubscribe(MsgID msgID, std::function<void(std::shared_ptr<Message>)> callback);

    public:
        Messenger();

        // Must be declared even if defaulted because Impl is an incomplete type here
        ~Messenger();

        template<typename TMessage>
        void subscribe(std::function<void(std::shared_ptr<Message>)> callback) {
            MsgID msgID = getMsgID<TMessage>();
            internalSubscribe(msgID, callback);
        }

        template<typename TMessage>
        void enqueue(const TMessage& msg) {
            MsgID msgID = getMsgID<TMessage>();
            std::shared_ptr<Message> m = std::static_pointer_cast<Message>(std::make_shared<TMessage>(msg));
            internalEnqueue(msgID, m);
        }

        void sendAll();
    };
}
#pragma warning(pop)