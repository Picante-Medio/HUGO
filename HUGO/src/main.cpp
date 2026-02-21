#include "SDL.h"
#include "Ogre.h"
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

#include "Core.h"
#include "Scene.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "systems/audio/AudioSystem.h"


#include <iostream>

int main(int argc, char* argv[]) {

    HUGO::Core core = HUGO::Core();
    core.init();
    core.run();
    core.destroy();
    return 0;
}
