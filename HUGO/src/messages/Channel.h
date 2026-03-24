#include "Message.h"
#include <queue>

namespace HUGO {
class IChannel {
public:
    virtual bool empty() const = 0;
    virtual Message pop() = 0;
    virtual void push(Message m) = 0;
};

template <typename TMessage>
class Channel : public IChannel {
    std::queue<TMessage> messages;

public:
    bool empty() const override { return messages.empty(); }

    Message pop() override { 
        Message m = messages.front();
        messages.pop();
        return m;
    }

    void push(Message m) override {
        messages.push(m);
    }
};
}
