#include "ConfigGenerator.h"

#include <cstdio>
#include <filesystem>

using std::filesystem::directory_entry;
using std::filesystem::directory_iterator;

void 
HUGO::ConfigGenerator::generateResourcesConfig() {
    // Delete resources config file if it exists
    if(std::filesystem::exists("resources.cfg"))
        std::remove("resources.cfg");

    // Create new file with the pre-config ready
    std::ofstream file("resources.cfg");
    file << "[General]\n";

    //Append engine and game resources
    if(std::filesystem::exists(RESOURCES))
        appendResources(RESOURCES, file);

    if(std::filesystem::exists(ASSETS))
        appendResources(ASSETS, file);

    //Close file before OGRE tries to read it
    file.close();
}

void
HUGO::ConfigGenerator::appendResources(const std::filesystem::path& path, std::ofstream& configFile) {
    // Zips must appear before their directories or it will lead to double loading
    for (directory_entry entry : directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".zip")
            configFile << "Zip=" << std::filesystem::canonical(entry.path()).string() << '\n';
    }

    // Recursion into directories
    for (directory_entry entry : directory_iterator(path)) {
        if (entry.is_directory()) {
            configFile << "FileSystem=" << std::filesystem::canonical(entry.path()).string() << '\n';
            appendResources(entry.path(), configFile);
        }
    }
}


void 
HUGO::ConfigGenerator::generatePluginsConfig() {
    // Delete plugins config if it exists
    if(std::filesystem::exists("plugins.cfg"))
        std::remove("plugins.cfg");

    // Add plugins
    std::ofstream file("plugins.cfg");
    for(auto plugin : requiredPlugins)
        file << "Plugin=" << plugin << '\n';

    // Close file before OGRE tries to read it
    file.close();
}
