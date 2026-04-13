#include "TestSystemVideo1.h"

#include "PanelComponent.h"
#include "ButtonComponent.h"
#include "TextComponent.h"
#include "SliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "AudioComponent.h"
#include "ChangeSceneMessage.h"
#include <iostream>

void
TestSystemVideo1::init() {
    scene.subscribe<ChangeSceneMessageVideo1>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessageVideo1> m = std::static_pointer_cast<ChangeSceneMessageVideo1>(msg);
        scene.changeAndDestroyPreviousScene(m->sceneName);
    });

    audio = scene.createEntity();
    scene.addComponent<AudioComponent>(audio, [](AudioComponent& a){
        a.isPlaying = true;
        a.type = HUGO::AudioTypes::MUSIC;
        a.audioName = "lucky";
        a.volume = 0.1f;
    });

    menuText = scene.createEntity();

    scene.addComponent<PanelComponent>(menuText, [](PanelComponent& panel3){
        panel3.overlayName = "Menu/Mierdas";
        panel3.containerName = panel3.overlayName + "/Container";
        panel3.material = "Console/InputBackground";
        panel3.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel3.width = 0.4f;
        panel3.height = 0.15f;
        panel3.left = 0.35f;
        panel3.top = 0.05f;
        panel3.zOrder = 500;
    });

    scene.addComponent<TextComponent>(menuText, [](TextComponent& text){
        text.left = 0.1f;
        text.top = 0.06f;
        text.colourTop = HUGO::Vector4(1.0f, 0.0f, 1.0f, 1.f);
        text.colourBottom = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text.characterHeight = 0.09f;
        text.spaceWidth = 0.01f;
        text.text = "H.U.G.O.";
    });

    menuButton = scene.createEntity();
    scene.addComponent<PanelComponent>(menuButton, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas2";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Knob";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.15f;
        panel.height = 0.15f;
        panel.left = 0.35f;
        panel.top = 0.5f;
        panel.isVisible = true;
        panel.zOrder = 504;
    });

    scene.addComponent<ButtonComponent>(menuButton, [this](ButtonComponent& b){
        b.normalMaterial = "UI/Knob";
        b.hoverMaterial = "UI/KnobHover";
        b.pressedMaterial = "UI/KnobPressed";
        b.onClickCallback = [this](){
            scene.sendMessage<ChangeSceneMessageVideo1>(ChangeSceneMessageVideo1{});
        };
    });

    EntityID buttonText = scene.createEntity();
    scene.addComponent<TextComponent>(buttonText, [](TextComponent& text){
        text.left = 0.45f;
        text.top = 0.62f;
        text.colourTop = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text.colourBottom = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.f);
        text.characterHeight = 0.03f;
        text.text = "Change Scene";
    });

    audioSlider = scene.createEntity();
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
            scene.modifyComponent<AudioComponent>(this->audio, [value](AudioComponent& a){
                a.volume = value;
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
}


void
TestSystemVideo1::onLoad() {

}


void
TestSystemVideo1::update(double dt) {
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::THREE)){
        std::cout << "Cambiando escena...\n";
        ChangeSceneMessage c;
        c.sceneName = "TestSceneVideo3";
        scene.sendMessage<ChangeSceneMessage>(c);
    }
}

void
TestSystemVideo1::fixedUpdate(double) {}
