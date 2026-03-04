#include "BulletModule.h"
#include "IPhysicsSimulation.h"

BulletModule::BulletModule()
: IPhysicsModule()
{

}

BulletModule::~BulletModule() {
    for(std::shared_ptr<IPhysicsSimulation> simulation : inactiveSimulations)
        simulation->destroySimulation();

    for(std::shared_ptr<IPhysicsSimulation> simulation : activeSimulations)
        simulation->destroySimulation();

}

void BulletModule::createSceneSimulation(std::shared_ptr<Scene> scene) {
    // create BulletSimulation and ad it to activeSimulations container
}

void BulletModule::destroySceneSimulation(std::shared_ptr<Scene> scene) {
    // remove BulletSimulation from its container
    // destroy simulation
}

void BulletModule::resumeSceneSimulation(std::shared_ptr<Scene> scene) {
    // find coresponding BulletSimulation in inactiveSimulations container
    // move it to activeSimulations container 
}

void BulletModule::pauseSceneSimulation(std::shared_ptr<Scene> scene) {
    // find coresponding BulletSimulation in activeSimulations container
    // move it to inactiveSimulations container 
}

void BulletModule::performSimulations(double dt) {
    for(std::shared_ptr<IPhysicsSimulation> simulation : activeSimulations)
        simulation->performSimulation(dt);
}