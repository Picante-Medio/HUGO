#include "Core.h"
#include "Ogre.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "Scene.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "ResourceManager.h"

using namespace HUGO;
Core::Core()
{
}

//Initialization of HUGO modules and loading of games dynamic lib
void Core::init(){
    vt.reset();
    initModules();
    
    scenePtr = std::make_shared<Scene>();
     std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(RenderSystem(scenePtr, m_ogreRoot, m_renderWindow));
    scenePtr->addSystem(renderSystem);

    m_resourceManager = std::make_shared<HUGO::ResourceManager>(maEngine, m_ogreRoot);

    renderSystem->createTestEntity();
}

//Destruction of HUGO modules. End of HUGO
void Core::destroy(){
    destroyOgre();
    destroyMiniaudio();
}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    initOgre();
    initMiniaudio();

}

void Core::initOgre()
{
    m_ogreRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");

    if (!m_ogreRoot->restoreConfig()) {
        Ogre::LogManager::getSingleton().logMessage("Initialize::configure_ogre => " +
        std::string("ERROR: unable to configure Ogre"));
    }
    //If there is another rendering system available, we could choose between one or another
    for(Ogre::RenderSystem* renderSys: m_ogreRoot->getAvailableRenderers()){
        if(renderSys->getName() == "OpenGL Rendering Subsystem"){
            m_ogreRoot->setRenderSystem(renderSys);
            break;
        }
    }

    m_ogreRoot->getRenderSystem()->setConfigOption("Full Screen", "No");

    locateResources();

    m_ogreRoot->initialise(false);

    // creation of the render window, with the size 1000x800 and not full screen
    Ogre::NameValuePairList params;
    m_renderWindow = m_ogreRoot->createRenderWindow(
        "Ogre Test Window",
        1000, // width
        800,  // height
        false,
        &params
    );
}
void HUGO::Core::locateResources()
{
    auto& rgm = Ogre::ResourceGroupManager::getSingleton();
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    Ogre::String sec, type, arch;
    for(auto& s : cf.getSettingsBySection()) {
        sec = s.first;
        for (auto& t : s.second)
        {
            type = t.first;
            arch = t.second;
            rgm.addResourceLocation(arch,type,sec);
        }
    }
}
void HUGO::Core::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//HUGO main loop
void Core::run(){
    bool exit = false;

	while (!exit) {
        vt.update();
		double starTime = vt.getDeltaTime();

        scenePtr->fixedUpdate(starTime);
        scenePtr->update(starTime);

        m_ogreRoot->renderOneFrame();
		double frameTime = vt.getDeltaTime() - starTime;
		if (frameTime < 10) SDL_Delay(10- frameTime);
	}
}


void Core::destroyOgre(){
    m_renderWindow->destroy();

    m_ogreRoot->shutdown();
}

void HUGO::Core::initMiniaudio()
{
    maEngine = std::make_shared<ma_engine>();
    ma_result result = ma_engine_init(nullptr, maEngine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";
}

void HUGO::Core::destroyMiniaudio()
{
    ma_engine_uninit(maEngine.get());
}