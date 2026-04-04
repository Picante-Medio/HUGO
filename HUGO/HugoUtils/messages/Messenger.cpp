#include "Messenger.h"
#include "Message.h"

namespace HUGO {

Messenger::Messenger() {
    for (Channel& channel : channels)
        channel = Channel();
}

void 
Messenger::sendAll() {
    for(MsgID id : activeChannels) 
        channels[id].sendMessages();

    activeChannels.clear();
}

};
