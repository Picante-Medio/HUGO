#include "TweenSystem.h"
#include "TweenComponent.h"

void TweenSystem::init() {
    
}
void TweenSystem::onLoad() {

}
void TweenSystem::update(double dt) {
    for(auto& pack : scene->query<TweenComponent>()) {
        TweenComponent& comp = pack.getDeprecated<TweenComponent&>();

        for(auto it = comp.currentTweens.begin(); it != comp.currentTweens.end();){

            ErrorOrType<bool> res = it->get()->tweenParameter(scene, static_cast<HUGO::Float>(dt));

            if(res.isError()){
                //Write in console the error
                it = comp.currentTweens.erase(it);
            }
            //If tween ended
            else if(res.getValue()){
                it = comp.currentTweens.erase(it);
            }
            else ++it;
        }
    }
}
void TweenSystem::fixedUpdate(double) {}