#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include <iostream>

// Aunque de warning esto no puede borrarse, en windows no compila porque SDL busca esta implementacion
int main(int argc, char* argv[]) {

    HUGO::Core core = HUGO::Core();
    ErrorOrVoid err = core.init();
    
    if(err.isError()) std::cerr << "[ERROR] Initializing HUGO::Core: {" << err.getError().errorMessage << "}. HUGO Engine will be closing now." << '\n';
    else{
        core.run();
    }
    return 0;
}