#pragma once

#include <filesystem>
#include <fstream>

class ResourcesGenerator {
    static void appendContent(const std::filesystem::path& path, std::ofstream& configFile);
public:
    ResourcesGenerator() = default;

    static void generateResourcesConfig();
};
