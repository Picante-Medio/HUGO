#include "AudioSystem.h"
#include <stdexcept>
#include <iostream>

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene): System(scene), 
    engine(std::make_shared<ma_engine>())
{
    ma_result result = ma_engine_init(nullptr, engine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";
}

AudioSystem::~AudioSystem(){
    //release all loaded audios
    /*for (auto a : audios) {
        ma_sound_uninit(a);
    } something like that*/
    ma_engine_uninit(engine.get());
}
void AudioSystem::update(double dt){
    /*
    get all the entities with an audio message to process and call handleTag()
    in the message it includes which type of sound we need to play
        handleTag();
    */
}