#pragma once

namespace HUGO {

    using MsgID = unsigned long int;

    // Empty struct, implementation must be provided by user per type of message
    struct Message {};

    const MsgID MAX_MESSAGES = 1024u;

    class MessageHandler{

        static inline MsgID MSGCounter() {
            static MsgID counter = 0;
            return counter++;
        }
        
    public:

        template <typename T>
        static inline MsgID getMsgID() {
            static MsgID id = MSGCounter();
            return id;
        }
    };
}
