#include "SDL.h"
#include "Ogre.h"
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "Core.h"
#include "Scene.h"

int main(int argc, char* argv[]) {

    HUGO::Core core = HUGO::Core();
    core.init();
    core.run();
    return 0;
}
