#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include <iostream>

int main(int, char*[]) {
    try {
        HUGO::Core *core = new HUGO::Core();
        HUGO::ErrorOrVoid error = core->run();

        if(error.isError()) 
            std::cerr << "[ERROR] Updating HUGO::Core: {" << error.getError().errorMessage 
            << "}. HUGO Engine will be closing now." << '\n';

        delete core;
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
