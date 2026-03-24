#include "Messenger.h"

namespace HUGO {

Messenger::Messenger() {
    for (Channel& channel : channels)
        channel = Channel();
}

void 
Messenger::sendAll() {
    for(Channel channel : channels) 
        channel.sendMessages();
}

};
