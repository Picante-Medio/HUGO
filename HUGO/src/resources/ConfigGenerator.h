#pragma once

#include <filesystem>
#include <fstream>

namespace HUGO {
class ConfigGenerator {
    const std::filesystem::path RESOURCES = std::filesystem::current_path() / ".." / "Resources";
    const std::filesystem::path ASSETS = std::filesystem::current_path() / "Assets";

    void appendResources(const std::filesystem::path& path, std::ofstream& configFile);
public:
    ConfigGenerator() = default;

    void generateResourcesConfig();
    void generatePluginsConfig();
};
}
