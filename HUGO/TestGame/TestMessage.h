#pragma once

#include <string>
#include "HUGO.h"

struct TestMessage : public HUGO::Message {
    std::string s = "AAAAAAAA FUNCIONA";
};