#pragma once

#include <filesystem>
#include <fstream>

namespace HUGO {
class ResourcesGenerator {
    const std::filesystem::path RESOURCES = std::filesystem::path("..") / "Resources";
    const std::filesystem::path ASSETS = std::filesystem::path("Assets");

    void appendContent(const std::filesystem::path& path, std::ofstream& configFile);
public:
    ResourcesGenerator() = default;

    void generateResourcesConfig();
};
}
