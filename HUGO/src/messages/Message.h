#pragma once
namespace HUGO {
using MsgID = unsigned long int;

// Empty struct, implementation must be provided by user per type of message
struct Message {

};

const MsgID MAX_MESSAGES = 1024u;

static MsgID counter = 0;

template<typename TMessage>
static MsgID getMsgID() {
    static MsgID msg = counter++; 
    return msg;
}
}
