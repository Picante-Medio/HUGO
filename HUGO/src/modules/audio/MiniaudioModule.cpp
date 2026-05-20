#include "MiniaudioModule.h"
#include "miniaudio.h"
#include <iostream>

std::unique_ptr<MiniaudioModule> MiniaudioModule::instanceInternal = 
    std::unique_ptr<MiniaudioModule>(new MiniaudioModule());


MiniaudioModule& 
MiniaudioModule::instance(){
    static MiniaudioModule& instance = *instanceInternal;
    return instance;
}

void
MiniaudioModule::destroyInstance() {
    if (!instanceInternal) return;
    instanceInternal.reset(nullptr);
}

MiniaudioModule::MiniaudioModule(){
    m_maEngine = std::make_unique<ma_engine>();
    ma_result result = ma_engine_init(nullptr, m_maEngine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";
}

MiniaudioModule::~MiniaudioModule(){
    ma_engine_uninit(m_maEngine.get());
}

ma_engine& MiniaudioModule::getMiniaudioEngine(){
    return *m_maEngine;
}