#include "ResourceLocator.h"
#include "HugoError.h"
#include "HugoTypes.h"

void 
HUGO::ResourceLocator::loadResources() {

}

ErrorOrType<HUGO::String> 
HUGO::ResourceLocator::getMeshAssetLocation(HUGO::String meshName) const {
    if(meshLocations.count(meshName)) 
        return ErrorOrType<HUGO::String>(meshLocations.at(meshName));
    else 
        return ErrorOrType<HUGO::String>(StringError(meshName +  " is not a valid mesh resource"));
}

ErrorOrType<HUGO::String> 
HUGO::ResourceLocator::getAudioAssetLocation(HUGO::String audioName) const {
    if(audioLocations.count(audioName)) 
        return ErrorOrType<HUGO::String>(audioLocations.at(audioName));
    else 
        return ErrorOrType<HUGO::String>(StringError(audioName +  " is not a valid audio resource"));
}
