#include "TestSystemVideo3.h"
#include "ChildrenComponent.h"
#include "EcsTypes.h"
#include "HugoMath.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"
#include "ChangeSceneMessage.h"
#include "AudioComponent.h"
#include "SliderComponent.h"
#include "PanelComponent.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "ButtonComponent.h"
#include "AnimationComponent.h"

struct Aux {
    HUGO::String pname = "Particle/Smoke";
    HUGO::Bool emmiting = true;
    HUGO::Bool active = true;
    HUGO::String material = "debug/Smoke";
};

void
TestSystemVideo3::init() {

    // CONSTANTES
    // -------------------------------------------------------------------------------------
    const Aux pInfo;
    constexpr HUGO::Float offset = 10.0f;
    // -------------------------------------------------------------------------------------

    // MENSAJES
    // -------------------------------------------------------------------------------------
    scene.subscribe<ChangeSceneMessageVideo3>([this](std::shared_ptr<HUGO::Message> msg) {
        std::shared_ptr<ChangeSceneMessageVideo3> m = std::static_pointer_cast<ChangeSceneMessageVideo3>(msg);
        scene.changeScene(m->sceneName);
    });
    // -------------------------------------------------------------------------------------


    // -------------------------------------------------------------------------------------

    // CUBOPEDRO
    // -------------------------------------------------------------------------------------
    cuboPedro = scene.createEntity();
    scene.addComponent<TransformComponent>(cuboPedro, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(-offset, 00.0f, 0.0f));
        transform.setScale(HUGO::Vector3(4, 4, 4));
    });

    scene.addComponent<RenderComponent>(cuboPedro, [](RenderComponent& r){
        r.meshName = "cube.mesh";
        r.materialName = "Square/Pedro";
    });

    scene.addComponent<ParticleEmitterComponent>(cuboPedro, [pInfo](ParticleEmitterComponent& pe){
        pe.particleName = pInfo.pname;
        pe.emitting = pInfo.emmiting;
        pe.active = pInfo.active;
        pe.material = pInfo.material;
        pe.quota = 200;
    });

    scene.addComponent<AudioComponent>(cuboPedro, [](AudioComponent& a){
        a.isPlaying = true;
        a.type = HUGO::AudioTypes::SOUND3D;
        a.audioName = "pedro";
        a.volume =0.1f;
        a.loop = true;
    });
    
    //---------------------------------------------------------------------------------------

    // CUBOPABLO
    // -------------------------------------------------------------------------------------
    cuboPablo = scene.createEntity();
    scene.addComponent<TransformComponent>(cuboPablo, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(offset, 0.0f, 0.0f));
        transform.setScale(HUGO::Vector3(4, 4, 4));
    });

    scene.addComponent<RenderComponent>(cuboPablo, [](RenderComponent& r){
        r.meshName = "cube.mesh";
        r.materialName = "Square/Pablo";
    });

    scene.addComponent<ParticleEmitterComponent>(cuboPablo, [pInfo](ParticleEmitterComponent& pe){
        pe.particleName = pInfo.pname;
        pe.emitting = pInfo.emmiting;
        pe.active = pInfo.active;
        pe.material = pInfo.material;
        pe.quota = 200;
    });

    scene.addComponent<AudioComponent>(cuboPablo, [](AudioComponent& a){
        a.isPlaying = true;
        a.type = HUGO::AudioTypes::SOUND3D;
        a.audioName = "pablo";
        a.volume =0.1f;
        a.loop = true;
    });
    // -------------------------------------------------------------------------------------

    // SINABD
    // -------------------------------------------------------------------------------------
    sinabd = scene.createEntity();

    scene.addComponent<TransformComponent>(sinabd);

    scene.addComponent<RenderComponent>(sinabd, [](RenderComponent& r) {
        r.meshName = "Sinbad.mesh";
    });

    scene.addComponent<AnimationComponent>(sinabd, [](AnimationComponent& a) {
        a.addAnimation("Dance", true, true);
    });

    scene.addComponent<ChildrenComponent>(sinabd, [this](ChildrenComponent& children) {
        children.addChild(cuboPablo);
        children.addChild(cuboPedro);
    });

    // -------------------------------------------------------------------------------------
    
    // AUDIO SLIDER
    // -------------------------------------------------------------------------------------
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
        s.trackElementName = "Menu/Mierdas34";
        s.knobElementName = s.trackElementName + "/Element";
        s.trackMaterial = "UI/Background";
        s.knobMaterial = "UI/KnobPressed";
        s.trackMaterial = "UI/Track";
        s.knobWidth = 0.05f;
        s.knobHeight = 0.05f;
        s.onChangeCallback = [this](HUGO::Float value){
            scene.modifyComponent<AudioComponent>(this->cuboPedro, [value](AudioComponent& a){
                a.volume = std::clamp(value, 0.0f, 1.0f);
            });

            scene.modifyComponent<AudioComponent>(this->cuboPablo, [value](AudioComponent& a){
                a.volume = std::clamp(value, 0.0f, 1.0f);
            });
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

    // -------------------------------------------------------------------------------------
    
    // NEXT BUTTON
    // -------------------------------------------------------------------------------------
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
    // -------------------------------------------------------------------------------------
}


void
TestSystemVideo3::onLoad() {}


void
TestSystemVideo3::update(uint32_t) {
    if(scene.getInput()->wasKeyJustPressed(HUGO::KeyCode::ONE)){
        std::cout << "Cambiando escena...\n";
        ChangeSceneMessage c;
        c.sceneName = "TestSceneVideo1";
        scene.sendMessage<ChangeSceneMessage>(c);
    }

    scene.modifyComponent<TransformComponent>(sinabd, [this](TransformComponent& t){
        t.setRotation(t.getRotation() * HUGO::Quaternion(HUGO::Radian(HUGO::Degree(rotationFactor)), HUGO::Vector3{0, 1, 0}));
    });
}

void
TestSystemVideo3::fixedUpdate(uint32_t) {}
