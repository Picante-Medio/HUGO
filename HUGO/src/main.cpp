#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include "ConfigGenerator.h"
#include <iostream>

int main(int, char*[]) {
    try {
        HUGO::ConfigGenerator config;
        config.generateResourcesConfig();
        config.generatePluginsConfig();

        HUGO::Core core = HUGO::Core();
        HUGO::ErrorOrVoid error = core.run();

        if(error.isError()) 
            std::cerr << "[ERROR] Updating HUGO::Core: {" << error.getError().errorMessage 
            << "}. HUGO Engine will be closing now." << '\n';
    }
    catch(std::exception e) {
        std::cerr << "[ERROR] Exception thrown: " << e.what() << '\n';
    }
    catch(std::string str){
        std::cerr << "[ERROR] Exception thrown: " << str << '\n';
    }
    catch(const char* str){
        std::cerr << "[ERROR] Exception thrown: " << str << '\n';
    }
    return 0;
}
