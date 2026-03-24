#include "Messenger.h"

namespace HUGO {

void 
Messenger::sendAll() {
    for(Channel channel : channels) 
        channel.sendMessages();
}

};
