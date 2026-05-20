#pragma once

#include "IPhysicsModule.h"

class BulletModule : public IPhysicsModule {

public:
    BulletModule();
    ~BulletModule();

    void createSceneSimulation(std::string sceneName, InternalScene& scene);
    void destroySceneSimulation(std::string sceneName);
    
    void resumeSceneSimulation(std::string sceneName);
    void pauseSceneSimulation(std::string sceneName);

    // sync info and step the simulation of active scenes
    void performSimulations(uint32_t dt) override;

    // physics visual debugger fro active scenes
    void debugDraw() override;
};