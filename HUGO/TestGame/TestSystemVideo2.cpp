#include "TestSystemVideo2.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "AnimationComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"


#include "ChangeSceneMessage.h"

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

    //Sinbad
    sinbad = scene.createEntity();
    scene.addComponent<TransformComponent>(sinbad, [](TransformComponent& tr){
        tr.setPosition(HUGO::Vector3(0.0f,20.f,0.0f));
    });

    scene.addComponent<AnimationComponent>(sinbad, [](AnimationComponent& anim){
        anim.addAnimation("RunBase", true, false);
        anim.addAnimation("RunTop", true, false);
    });

    scene.addComponent<RenderComponent>(sinbad, [](RenderComponent& rc){
        rc.setMeshName("Sinbad.mesh");
    });

    scene.addComponent<ColliderComponent>(sinbad, [](ColliderComponent& collider){
        collider.info = HUGO::BoxColliderInfo{HUGO::Vector3(2.f, 4.2f, 2.f)};
        collider.positionOffset=HUGO::Vector3(0.0f, -1.f, 0.0f);
    });

    scene.addComponent<RigidBodyComponent>(sinbad, [](RigidBodyComponent& rb){
        rb.setMass(5.f);
        rb.setBodyType(HUGO::BodyType::DYNAMIC);
    });


    //Suelo
    suelo = scene.createEntity();
    scene.addComponent<TransformComponent>(suelo, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0.0f,-10.f,0.0f));
        transform.setScale(HUGO::Vector3(40.f, 0.5f, 40.f));
    });
    createCube(suelo);

    //pared fondo
    EntityID paredFondo = scene.createEntity();
    scene.addComponent<TransformComponent>(paredFondo, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0.0f,10.f,-20.0f));
        transform.setScale(HUGO::Vector3(40.f, 0.5f, 40.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(1,0,0)));
    });
    createCube(paredFondo);

    //pared der
    EntityID paredDer = scene.createEntity();
    scene.addComponent<TransformComponent>(paredDer, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(20.0f,10.f,0.0f));
        transform.setScale(HUGO::Vector3(40.f, 0.5f, 40.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(1,0,0)) * HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(0,0,1)));
    });
    createCube(paredDer);

    //pared izq
    EntityID paredIzq = scene.createEntity();
    scene.addComponent<TransformComponent>(paredIzq, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(-20.0f,10.f,0.0f));
        transform.setScale(HUGO::Vector3(40.f, 0.5f, 40.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(1,0,0)) * HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(0,0,1)));
    });
    createCube(paredIzq);

    EntityID collider = scene.createEntity();
    scene.addComponent<TransformComponent>(collider, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0.0f,10.f,20.0f));
        transform.setScale(HUGO::Vector3(40.f, 0.5f, 40.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90.0f)), HUGO::Vector3(1,0,0)));
    });
    scene.addComponent<ColliderComponent>(collider, [](ColliderComponent& collider){
        collider.info = HUGO::BoxColliderInfo{HUGO::Vector3(20.f, 0.5f, 20.f)};
        collider.setFriction(0.5f);
        collider.setRestitution(0.5f);
    });
    scene.addComponent<RigidBodyComponent>(collider, [](RigidBodyComponent& rb){
        rb.setBodyType(HUGO::BodyType::STATIC);
    });

    //cubo
    EntityID cubo = scene.createEntity();
    scene.addComponent<TransformComponent>(cubo, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(5.0f,10.f,0.0f));
        transform.setScale(HUGO::Vector3(2.f, 2.f, 2.f));
    });
    scene.addComponent<RenderComponent>(cubo, [](RenderComponent& rc){
        rc.setMeshName("cube.mesh");
        rc.setMaterialName("prueba/ColorVerde");
    });

    scene.addComponent<ColliderComponent>(cubo, [](ColliderComponent& collider){
        collider.info = HUGO::BoxColliderInfo{HUGO::Vector3(1.f, 1.0f, 1.f)};
    });
    scene.addComponent<RigidBodyComponent>(cubo, [](RigidBodyComponent& rb){
        rb.setMass(1.f);
        rb.setBodyType(HUGO::BodyType::DYNAMIC);
    });

    ///luz
    EntityID light = scene.createEntity();
    scene.addComponent<TransformComponent>(light, [](TransformComponent& tr){
        tr.setPosition(HUGO::Vector3(0, 0, 0));
    });

    scene.addComponent<LightComponent>(light, [](LightComponent& lg){
         lg.setLightType(HUGO::LightType::LT_POINT);
        lg.setAttenuation(HUGO::Vector4(1.0f, 0.0f, 0.001f, 0.0001f));
         lg.setDiffuse(HUGO::Vector4(0.8f, 0.8f, 0.8f, 1.0f));
         lg.setSpecular(HUGO::Vector4(0.3f, 0.3f, 0.3f, 1.0f));
     });
}

void TestSystemVideo2::createCube(EntityID cube){
    scene.addComponent<RenderComponent>(cube, [](RenderComponent& rc){
        rc.setMeshName("cube.mesh");
        rc.setMaterialName("prueba/ColorSuelo");
    });

    scene.addComponent<ColliderComponent>(cube, [](ColliderComponent& collider){
        collider.info = HUGO::BoxColliderInfo{HUGO::Vector3(20.f, 0.5f, 20.f)};
        collider.setFriction(0.1f);
    });

    scene.addComponent<RigidBodyComponent>(cube, [](RigidBodyComponent& rb){
        rb.setBodyType(HUGO::BodyType::STATIC);
    });
}


void
TestSystemVideo2::onLoad() {}


void
TestSystemVideo2::update(double dt) {

    auto input = scene.getInput();
    float delta = static_cast<float>(dt);
    HUGO::Quaternion rot = scene.getComponentConst<TransformComponent>(sinbad).getRotation();
    scene.modifyComponent<RigidBodyComponent>(sinbad, [input, delta, rot](RigidBodyComponent& rb) {
        
        if(input->isKeyPressed(HUGO::KeyCode::LEFT_ARROW)) rb.setAngularVelocity(HUGO::Vector3(0, 1.0f, 0));
        else if(input->isKeyPressed(HUGO::KeyCode::RIGHT_ARROW)) rb.setAngularVelocity(HUGO::Vector3(0, -1.0f, 0));
        else rb.setAngularVelocity(HUGO::Vector3());

        if(input->isKeyPressed(HUGO::KeyCode::DOWN_ARROW))rb.applyForce(rot.forward() * (25.0f)* delta);
        if(input->isKeyPressed(HUGO::KeyCode::UP_ARROW))rb.applyForce(rot.forward() * (-25.0f)* delta);
        if(input->isKeyPressed(HUGO::KeyCode::M)) 
            rb.applyForce(rot.up() * (50.0f)* delta);
    });

    scene.modifyComponent<AnimationComponent>(sinbad, [input](AnimationComponent& anim) {
        if(input->wasKeyJustReleased(HUGO::KeyCode::DOWN_ARROW) || 
            input->wasKeyJustReleased(HUGO::KeyCode::UP_ARROW)|| 
            input->wasKeyJustReleased(HUGO::KeyCode::LEFT_ARROW)||
            input->wasKeyJustReleased(HUGO::KeyCode::RIGHT_ARROW)) 
        {
            anim.setEnabled("RunBase", false);
            anim.setEnabled("RunTop", false);
        }
        if(input->wasKeyJustPressed(HUGO::KeyCode::DOWN_ARROW) || 
            input->wasKeyJustPressed(HUGO::KeyCode::UP_ARROW)|| 
            input->wasKeyJustPressed(HUGO::KeyCode::LEFT_ARROW)||
            input->wasKeyJustPressed(HUGO::KeyCode::RIGHT_ARROW)) 
        {
            anim.setEnabled("RunBase", true);
            anim.setEnabled("RunTop", true);
        }
    });
}

void
TestSystemVideo2::fixedUpdate(double) {}
