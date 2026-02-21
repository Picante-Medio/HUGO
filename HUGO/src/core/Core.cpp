#include "Core.h"
#include "Ogre.h"

using namespace HUGO;


Core::Core(){
    
}

//Inicialización de los módulos de HUGO y carga de la .dll del juego
void Core::init(){
    initOgre();
}

//Destrucción de los módulos del motor. Final de HUGO
void Core::destroy(){
    destroyOgre();
}

//Bucle principal de HUGO
void Core::run(){

}

//--------------------------- INICIALIZACIÓN MÓDULOS --------------------

void Core::initOgre(){
    m_ogreRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");

        
    for(Ogre::RenderSystem* renderSys: m_ogreRoot->getAvailableRenderers()){
        if(renderSys->getName() == "OpenGL Rendering Subsystem"){
            m_ogreRoot->setRenderSystem(renderSys);
            break;
        }
    }
    m_renderWindow = m_ogreRoot->initialise(true, "Prueba Ogre");

    m_ogreRoot->getRenderSystem()->setConfigOption("Full Screen", "Yes");
}

void Core::destroyOgre(){

}