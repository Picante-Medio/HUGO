#pragma once

#include "IPhysicsModule.h"

class BulletModule : public IPhysicsModule {

public:
    BulletModule();
    ~BulletModule();

    void createSceneSimulation(std::shared_ptr<Scene> scene);
    void destroySceneSimulation(std::shared_ptr<Scene> scene);
    
    void resumeSceneSimulation(std::shared_ptr<Scene> scene);
    void pauseSceneSimulation(std::shared_ptr<Scene> scene);

    // sync info and step the simulation of active scenes
    void performSimulations(double dt) override;
};