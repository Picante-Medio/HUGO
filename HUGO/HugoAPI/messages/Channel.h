#pragma once

#include "UtilsAPI.h"

#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "Message.h"

namespace HUGO {

    class UTILS_API Channel {
        // pImpl idiom to hide implementation details
        struct Impl;
        Impl *pImpl; 
        // Must be raw pointer to prevent warning (4251) and to prevent 
        // exporting interface of stl shared pointers

    public:
        Channel();

        // Must be declared even if defaulted because Impl is an incomplete type here
        ~Channel();

        void sendMessages();
        void subscribeCallback(std::function<void(std::shared_ptr<Message>)> callback);
        
        void enqueueMessage(std::shared_ptr<Message> msg);
    };
}