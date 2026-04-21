#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace HUGO {
class ConfigGenerator {
    // Path to engine resources
    const std::filesystem::path RESOURCES = std::filesystem::current_path() / ".." / "Resources";

    // Path to game resources
    const std::filesystem::path ASSETS = std::filesystem::current_path() / "Assets";

    // Plugins required for engine functionality
    const std::vector<std::string> requiredPlugins = {
        "RenderSystem_GL",
        "Codec_STBI",
        "Plugin_ParticleFX",
        "Codec_Assimp",
    };

    void appendResources(const std::filesystem::path& path, std::ofstream& configFile);
public:
    ConfigGenerator() = default;

    void generateResourcesConfig();
    void generatePluginsConfig();
};
}
