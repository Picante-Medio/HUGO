#include "Messenger.h"

namespace HUGO {

Messenger::Messenger()
: channels()
{

}

void 
Messenger::sendAll() {
    for(Channel channel : channels) 
        channel.sendMessages();
}

};
