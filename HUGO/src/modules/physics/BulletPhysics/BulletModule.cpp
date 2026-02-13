#include "BulletModule.h"
#include "IPhysicsSimulation.h"
#include "BulletSimulation.h"

BulletModule::BulletModule()
: IPhysicsModule()
{

}

BulletModule::~BulletModule() {

    // These calls are done by BulletSimulation in its destructor. calling it again will result in an unhandled exception
    // while trying to delete a deleted world

    /* for(std::shared_ptr<IPhysicsSimulation> simulation : inactiveSimulations)
        simulation->destroySimulation();

    for(std::shared_ptr<IPhysicsSimulation> simulation : activeSimulations)
        simulation->destroySimulation();*/

}

void BulletModule::createSceneSimulation(std::shared_ptr<Scene> scene) {
    // create BulletSimulation and add it to activeSimulations container
    std::shared_ptr<IPhysicsSimulation> simulation = std::make_shared<BulletSimulation>(scene);
    activeSimulations.push_back(simulation);
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