#include "BulletModule.h"
#include "IPhysicsSimulation.h"
#include "BulletSimulation.h"

BulletModule::BulletModule()
: IPhysicsModule()
{

}

BulletModule::~BulletModule() {

}

void BulletModule::createSceneSimulation(std::string sceneName, InternalScene& scene) {
    // create BulletSimulation and add it to activeSimulations container
    std::shared_ptr<IPhysicsSimulation> simulation = std::make_shared<BulletSimulation>(scene);
    activeSimulations.insert({sceneName, simulation});
}

void BulletModule::destroySceneSimulation(std::string sceneName) {
    // remove BulletSimulation from its container
    activeSimulations.erase(sceneName);
}

void BulletModule::resumeSceneSimulation(std::string sceneName) {
    // find coresponding BulletSimulation in inactiveSimulations container
    // move it to activeSimulations container 
    auto node = inactiveSimulations.extract(sceneName);
    if(node)
        activeSimulations.insert({node.key(), node.mapped()});
}

void BulletModule::pauseSceneSimulation(std::string sceneName) {
    // find coresponding BulletSimulation in activeSimulations container
    // move it to inactiveSimulations container 
    auto node = activeSimulations.extract(sceneName);
    if(node)
        inactiveSimulations.insert({node.key(), node.mapped()});
}

void BulletModule::performSimulations(uint32_t dt) {
    for(auto it = activeSimulations.begin(); it != activeSimulations.end(); ++it)
        it->second->performSimulation(dt);
}

void BulletModule::debugDraw() {
    for(auto it = activeSimulations.begin(); it != activeSimulations.end(); ++it)
        it->second->debugDraw();
}