#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include "ConfigGenerator.h"
#include <iostream>

// Aunque de warning esto no puede borrarse, en windows no compila porque SDL busca esta implementacion
int main(int, char*[]) {
    try {
    HUGO::ConfigGenerator config;
    config.generateResourcesConfig();
    config.generatePluginsConfig();

    HUGO::Core core = HUGO::Core();
    HUGO::ErrorOrVoid err = core.init();
    
    if(err.isError()) std::cerr << "[ERROR] Initializing HUGO::Core: {" << err.getError().errorMessage << "}. HUGO Engine will be closing now." << '\n';
    else{
            HUGO::ErrorOrVoid error = core.run();
            if(error.isError()) std::cerr << "[ERROR] Updating HUGO::Core: {" << error.getError().errorMessage << "}. HUGO Engine will be closing now." << '\n';
    }
    }
    catch(std::exception e) {
        std::cerr << e.what();
    }
    return 0;
}
