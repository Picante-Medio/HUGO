#pragma once
#include <memory>

struct ma_engine;

class MiniaudioModule{
    std::shared_ptr<ma_engine> m_maEngine;

    MiniaudioModule();
public:
    static MiniaudioModule& Instance();
    ~MiniaudioModule();

    std::shared_ptr<ma_engine> getMiniaudioEngine();
};