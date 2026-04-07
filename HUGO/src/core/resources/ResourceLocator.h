#pragma once

#include <unordered_map>
#include <string>

#include "HugoTypes.h"
#include "HugoError.h"

namespace HUGO {
class ResourceLocator {
    std::unordered_map<HUGO::String, HUGO::String> meshLocations;
    std::unordered_map<HUGO::String, HUGO::String> audioLocations;

public:
    ResourceLocator() = default;

    void loadResources();
    ErrorOrType<HUGO::String> getMeshAssetLocation(HUGO::String meshName) const;
    ErrorOrType<HUGO::String> getAudioAssetLocation(HUGO::String audioName) const;
};
}
