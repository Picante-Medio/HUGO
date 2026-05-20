#pragma once
#include <memory>

struct ma_engine;

class MiniaudioModule{
    std::shared_ptr<ma_engine> m_maEngine;

    MiniaudioModule();

    static std::unique_ptr<MiniaudioModule> instanceInternal;
public:
    static MiniaudioModule& instance();
    static void destroyInstance();

    ~MiniaudioModule();

    std::shared_ptr<ma_engine> getMiniaudioEngine();
};