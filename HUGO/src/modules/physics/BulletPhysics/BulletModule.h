#pragma once

#include "IPhysicsModule.h"

class BulletModule : public IPhysicsModule {

public:
    BulletModule();
    ~BulletModule();

    void createSceneSimulation(const std::string& sceneName, InternalScene& scene) override;
    void destroySceneSimulation(const std::string& sceneName) override;
    
    void resumeSceneSimulation(const std::string& sceneName) override;
    void pauseSceneSimulation(const std::string& sceneName) override;

    // sync info and step the simulation of active scenes
    void performSimulations(uint32_t dt) override;

    // physics visual debugger fro active scenes
    void debugDraw() override;
};