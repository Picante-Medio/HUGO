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
#include "CameraComponent.h"
#include "WorldTransformComponent.h"
#include "Audio3DInfoComponent.h"

void
TestSystemVideo1::init() {
    scene.subscribe<ChangeSceneMessageVideo1>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessageVideo1> m = std::static_pointer_cast<ChangeSceneMessageVideo1>(msg);
        scene.changeScene(m->sceneName);
    });

    audio = scene.createEntity();
    scene.addComponent<AudioComponent>(audio, [](AudioComponent& a){
        a.isPlaying = true;
        a.audioName = "funkytown";
        a.volume = 0.0f;
    });
    scene.addComponent<Audio3DInfoComponent>(audio);

    menuFondo = scene.createEntity();

    scene.addComponent<PanelComponent>(menuFondo, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas5";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Background";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 1.0f;
        panel.height = 1.0f;
        panel.left = 0.0f;
        panel.top = 0.0f;
        panel.zOrder = 500;
    });

    menuText = scene.createEntity();

    scene.addComponent<PanelComponent>(menuText, [](PanelComponent& panel3){
        panel3.overlayName = "Menu/Mierdas";
        panel3.containerName = panel3.overlayName + "/Container";
        panel3.material = "UI/HUGOwhite";
        panel3.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel3.width = 0.60f;
        panel3.height = 0.34f;
        panel3.left = 0.2f;
        panel3.top = 0.05f;
        panel3.zOrder = 501;
    });
    
    menuButton = scene.createEntity();
    scene.addComponent<PanelComponent>(menuButton, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas2";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Knob";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.28f;
        panel.height = 0.18f;
        panel.left = 0.4f;
        panel.top = 0.5f;
        panel.isVisible = true;
        panel.zOrder = 502;
    });

    scene.addComponent<ButtonComponent>(menuButton, [this](ButtonComponent& b){
        b.normalMaterial = "UI/HUGOPlayBoton";
        b.hoverMaterial = "UI/HUGOPlayBotonHover";
        b.pressedMaterial = "UI/HUGOPlayBotonHover";
        b.onClickCallback = [this](){
            scene.sendMessage<ChangeSceneMessageVideo1>(ChangeSceneMessageVideo1{});
        };
    });


    audioSlider = scene.createEntity();
    scene.addComponent<PanelComponent>(audioSlider, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas3";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/Background";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.25f;
        panel.height = 0.1f;
        panel.left = 0.4f;
        panel.top = 0.8f;
        panel.isVisible = true;
        panel.zOrder = 501;
    });

    scene.addComponent<SliderComponent>(audioSlider, [this](SliderComponent& s){
        s.trackElementName = "Menu/Mierdas4";
        s.knobElementName = s.trackElementName + "/Element";
        s.trackMaterial = "UI/Track";
        s.knobMaterial = "UI/TrackHUGO";
        s.knobWidth = 0.1f;
        s.knobHeight = 0.15f;
        s.knobTop = -0.025f;
        s.knobLeft = -0.025f;
        s.onChangeCallback = [this](HUGO::Float value){
            scene.modifyComponent<AudioComponent>(this->audio, [value](AudioComponent& a){
                a.volume = value;
            });
        };
    });

    auto camID = scene.createEntity();
    scene.addComponent<WorldTransformComponent>(camID);
    scene.addComponent<CameraComponent>(camID, [](CameraComponent& cmp){
        cmp.setMain(true);
    });
}


void
TestSystemVideo1::onLoad() {

}


void
TestSystemVideo1::update(uint32_t) {
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::THREE)){
        std::cout << "Cambiando escena...\n";
        ChangeSceneMessage c;
        c.sceneName = "TestSceneVideo3";
        scene.sendMessage<ChangeSceneMessage>(c);
    }
}

void
TestSystemVideo1::fixedUpdate(uint32_t) {}
