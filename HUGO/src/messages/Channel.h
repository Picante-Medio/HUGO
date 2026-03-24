#include <functional>
#include <memory>
#include <queue>
#include <vector>

#include "Message.h"

namespace HUGO {

class Channel {
    std::queue<Message> messages;
    std::vector<std::function<void(std::shared_ptr<Message>)>> callbacks;

public:
    Channel();
    void sendMessages();
    void subscribeCallback(std::function<void(std::shared_ptr<Message>)> callback);
};
}
