#include "TweenSystem.h"
#include "TweenComponent.h"

void TweenSystem::init() {
    
}
void TweenSystem::onLoad() {

}
void TweenSystem::update(uint32_t dt) {
    for(auto& pack : scene.query<TweenComponent>()) {
        pack.modify<TweenComponent>([this, dt](TweenComponent& comp){
            for(auto it = comp.currentTweens.begin(); it != comp.currentTweens.end();){

            HUGO::ErrorOrType<bool> res = it->get()->tweenParameter(scene, static_cast<HUGO::Float>(dt));

            if(res.isError()){
                scene.log(HUGO::StringError("[TweenSystem] " + res.getError().errorMessage));
                //Write in console the error
                it = comp.currentTweens.erase(it);
            }
            //If tween ended
            else if(res.getValue()){
                it = comp.currentTweens.erase(it);
            }
            else ++it;
        }
        });
    }
}
void TweenSystem::fixedUpdate(uint32_t) {}