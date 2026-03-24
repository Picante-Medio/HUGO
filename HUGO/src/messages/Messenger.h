#include <functional>
#include <memory>
#include <array>

#include "Channel.h"
#include "Message.h"

namespace HUGO {
class Messenger {
    std::array<Channel, MAX_MESSAGES> channels;

public:
    Messenger();

    template<typename T>
    void subscribe(std::function<void(std::shared_ptr<Message>)> callback) {
        MsgID msgID = getMsgID<T>();
        channels[msgID].subscribeCallback(callback);
    }

    void sendAll();
};
}
