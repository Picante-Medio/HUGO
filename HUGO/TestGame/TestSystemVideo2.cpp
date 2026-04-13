#include "TestSystemVideo2.h"

#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "ChangeSceneMessage.h"
#include "AudioComponent.h"
#include "Audio3DInfoComponent.h"
#include "SliderComponent.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "ButtonComponent.h"

#include <iostream>

void
TestSystemVideo2::init() {
    scene.subscribe<ChangeSceneMessageVideo2>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessageVideo2> m = std::static_pointer_cast<ChangeSceneMessageVideo2>(msg);
        scene.changeAndDestroyPreviousScene(m->sceneName);
    });

    menuButton = scene.createEntity();
    scene.addComponent<PanelComponent>(menuButton, [](PanelComponent& panel){
        panel.overlayName = "Menu/Mierdas77";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/HUGOPlayBoton";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.2f;
        panel.height = 0.1f;
        panel.left = 0.1f;
        panel.top = 0.1f;
        panel.isVisible = true;
        panel.zOrder = 540;
    });

    scene.addComponent<ButtonComponent>(menuButton, [this](ButtonComponent& b){
        b.normalMaterial = "UI/HUGOPlayBoton";
        b.hoverMaterial = "UI/HUGOPlayBotonHover";
        b.pressedMaterial = "UI/HUGOPlayBotonHover";
        b.onClickCallback = [this](){
            scene.sendMessage<ChangeSceneMessageVideo2>(ChangeSceneMessageVideo2{});
        };
    });
}


void
TestSystemVideo2::onLoad() {}


void
TestSystemVideo2::update(double dt) {
}

void
TestSystemVideo2::fixedUpdate(double) {}
