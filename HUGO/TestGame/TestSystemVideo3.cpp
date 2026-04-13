#include "TestSystemVideo3.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "ChangeSceneMessage.h"
#include "AudioComponent.h"
#include "Audio3DInfoComponent.h"
#include "SliderComponent.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"

struct Aux {
    HUGO::String pname = "Particle/Smoke";
    HUGO::Bool emmiting = true;
    HUGO::Bool active = true;
    HUGO::String material = "debug/Smoke";
};

void
TestSystemVideo3::init() {
    scene.subscribe<ChangeSceneMessageVideo3>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessageVideo3> m = std::static_pointer_cast<ChangeSceneMessageVideo3>(msg);
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
        a.audioName = "pablo";
        a.volume =0.1f;
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

    EntityID audioSlider = scene.createEntity();
    scene.addComponent<PanelComponent>(audioSlider, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas3";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Background";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.25f;
        panel.height = 0.05f;
        panel.left = 0.4f;
        panel.top = 0.8f;
        panel.isVisible = true;
        panel.zOrder = 501;
    });

    scene.addComponent<SliderComponent>(audioSlider, [this](SliderComponent& s){
        s.trackElementName = "Menu/Mierdas4";
        s.knobElementName = s.trackElementName + "/Element";
        s.trackMaterial = "UI/Background";
        s.knobMaterial = "UI/KnobPressed";
        s.trackMaterial = "UI/Track";
        s.knobWidth = 0.05f;
        s.knobHeight = 0.05f;
        s.onChangeCallback = [this](HUGO::Float value){
            scene.modifyComponent<AudioComponent>(this->ent2, [value](AudioComponent& a){
                a.volume = std::clamp(value, 0.0f, 1.0f);
            });
            std::cout << "Volume: " << value << "\n";
        };
    });

    EntityID volumeText = scene.createEntity();
    scene.addComponent<TextComponent>(volumeText, [](TextComponent& text){
        text.left = 0.1f;
        text.top = 0.06f;
        text.colourTop = HUGO::Vector4(1.0f, 0.0f, 1.0f, 1.f);
        text.colourBottom = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text.characterHeight = 0.09f;
        text.spaceWidth = 0.01f;
        text.text = "Volume";
    });

    EntityID menuButton = scene.createEntity();
    scene.addComponent<PanelComponent>(menuButton, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas10";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/HUGOPlayBoton";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.2f;
        panel.height = 0.1f;
        panel.left = 0.7f;
        panel.top = 0.1f;
        panel.isVisible = true;
        panel.zOrder = 540;
    });

    scene.addComponent<ButtonComponent>(menuButton, [this](ButtonComponent& b){
        b.normalMaterial = "UI/HUGOPlayBoton";
        b.hoverMaterial = "UI/HUGOPlayBotonHover";
        b.pressedMaterial = "UI/HUGOPlayBotonHover";
        b.onClickCallback = [this](){
            scene.sendMessage<ChangeSceneMessageVideo3>(ChangeSceneMessageVideo3{});
        };
    });
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
