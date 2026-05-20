#include "MiniaudioModule.h"
#include "miniaudio.h"
#include <iostream>

std::unique_ptr<MiniaudioModule> MiniaudioModule::instanceInternal = std::make_unique<MiniaudioModule>();


MiniaudioModule& 
MiniaudioModule::instance(){
    static MiniaudioModule& instance = *instanceInternal;
    return instance;
}

void
MiniaudioModule::destroyInstance() {
    if (!instanceInternal) return;
    instanceInternal.reset();
}

MiniaudioModule::MiniaudioModule(){
    m_maEngine = std::make_shared<ma_engine>();
    ma_result result = ma_engine_init(nullptr, m_maEngine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";
}

MiniaudioModule::~MiniaudioModule(){
    ma_engine_uninit(m_maEngine.get());
}

std::shared_ptr<ma_engine> MiniaudioModule::getMiniaudioEngine(){
    return m_maEngine;
}