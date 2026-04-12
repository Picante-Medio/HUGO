#include "TestSystemVideo3.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "ChangeSceneMessage.h"
#include "AudioComponent.h"
#include "Audio3DInfoComponent.h"

struct Aux {
    HUGO::String pname = "Particle/Smoke";
    HUGO::Bool emmiting = true;
    HUGO::Bool active = true;
    HUGO::String material = "debug/Smoke";
};

void
TestSystemVideo3::init() {
    scene.subscribe<ChangeSceneMessage>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessage> m = std::static_pointer_cast<ChangeSceneMessage>(msg);
        scene.changeScene(m->sceneName);
    });

    Aux a;
    HUGO::Float offset = 10.0f;

    ent1 = scene.createEntity();
    scene.addComponent<TransformComponent>(ent1, [offset](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(offset, 0.0f, 0.0f));
    });

    scene.addComponent<ParticleEmitterComponent>(ent1, [a](ParticleEmitterComponent& pe){
        pe.particleName = a.pname;
        pe.emitting = a.emmiting;
        pe.active = a.active;
        pe.material = a.material;
        pe.quota = 200;
    });

    ent2 = scene.createEntity();

    scene.addComponent<TransformComponent>(ent2, [offset](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(-offset, 0.0f, 0.0f));
    });

    scene.addComponent<ParticleEmitterComponent>(ent2, [a](ParticleEmitterComponent& pe){
        pe.particleName = a.pname;
        pe.emitting = a.emmiting;
        pe.active = a.active;
        pe.material = a.material;
        pe.quota = 200;
    });
    scene.addComponent<AudioComponent>(ent2, [](AudioComponent& a){
        a.isPlaying = true;
        a.type = HUGO::AudioTypes::MUSIC;
        a.audioName = "lucky";
        a.volume =0.0f;
        a.loop = true;
    });
    // scene.addComponent<Audio3DInfoComponent>(ent2);

    // EntityID audio = scene.createEntity();
    // scene.addComponent<AudioComponent>(audio, [](AudioComponent& a){
    //     a.isPlaying = true;
    //     a.type = HUGO::AudioTypes::MUSIC;
    //     a.audioName = "lucky";
    //     a.volume =0.1f;
    // });


}


void
TestSystemVideo3::onLoad() {}


void
TestSystemVideo3::update(double dt) {
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::ONE)){
        std::cout << "Cambiando escena...\n";
        ChangeSceneMessage c;
        c.sceneName = "TestSceneVideo1";
        scene.sendMessage<ChangeSceneMessage>(c);
    }
    
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::P)){
        scene.modifyComponent<AudioComponent>(ent2, [](AudioComponent& a){
            a.volume += 0.05f;
        });
    }
}

void
TestSystemVideo3::fixedUpdate(double) {}
