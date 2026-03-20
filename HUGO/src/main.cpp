#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include <iostream>

int main() {

    HUGO::Core core = HUGO::Core();
    ErrorOrVoid err = core.init();
    
    if(err.isError()) std::cerr << "[ERROR] Initializing HUGO::Core: {" << err.getError().errorMessage << "}. HUGO Engine will be closing now." << '\n';
    else{
        core.run();
    }
    return 0;
}
