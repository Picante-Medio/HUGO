#include <memory>
#include <array>

#include "Channel.h"
#include "Message.h"
#include "System.h"

namespace HUGO {
class Messenger {
    std::array<std::shared_ptr<IChannel>, MAX_MESSAGES> channels;

    template<typename TMessage> 
    void ensureExists() {
		MsgID msgID = getMsgID<TMessage>();
		if(channels[msgID] != nullptr) return;
		channels[msgID] = std::make_shared<Channel<TMessage>>();
    }

    template<typename TMessage>
    std::shared_ptr<Channel<TMessage>> getChannel() {
        ensureExists<TMessage>();
        MsgID msgID = getMsgID<TMessage>();
        return std::static_pointer_cast<Channel<TMessage>>(channels[msgID]);
    }
public:
    template<typename TMessage>
    void send(std::vector<std::shared_ptr<System>> systems) {
        std::shared_ptr<Channel<TMessage>> channel = getChannel<TMessage>();
        while(!channel->empty()) {
            TMessage message = static_cast<TMessage>(channel->pop());
            for(auto system : systems) {
                system->receiveMessage<TMessage>(message);
            }
        }
    }

    template<typename TMessage>
    void enqueueMessage(TMessage message) {
        getChannel<TMessage>().push(message);
    }

    void sendAll() {}
};
}
