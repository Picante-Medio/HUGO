#include "TestSystemVideo2.h"

#include "AudioComponent.h"
#include "ChangeSceneMessage.h"
#include "PanelComponent.h"
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
        panel.overlayName = "Menu/Mierdas6";
        panel.containerName = panel.overlayName + "/Container";
        panel.material = "UI/HUGOPlayBoton";
        panel.metricsMode = HUGO::UIMetricsMode::GMM_RELATIVE;
        panel.width = 0.28f;
        panel.height = 0.18f;
        panel.left = 0.4f;
        panel.top = 0.5f;
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
    std::cout << "Updating TestSystemVideo2... dt: " << dt << "ms\n";
}

void
TestSystemVideo2::fixedUpdate(double) {}
