#include "ResourcesGenerator.h"

#include <cstdio>
#include <filesystem>

using std::filesystem::directory_entry;
using std::filesystem::directory_iterator;

void 
HUGO::ResourcesGenerator::generateResourcesConfig() {
    // Delete resources config file if it exists
    if(std::filesystem::exists("resources.cfg"))
        std::remove("resources.cfg");

    // Create new file with the pre-config ready
    std::ofstream file("resources.cfg");
    file << "[General]\n";

    //Append engine and game resources
    if(std::filesystem::exists(RESOURCES))
        appendContent(RESOURCES, file);

    if(std::filesystem::exists(ASSETS))
        appendContent(ASSETS, file);

    //Close file before OGRE tries to read it
    file.close();
}

void
HUGO::ResourcesGenerator::appendContent(const std::filesystem::path& path, std::ofstream& configFile) {
    for (directory_entry entry : directory_iterator(path)) {
        if(entry.is_directory()) {
            configFile << "FileSystem=" << entry.path().c_str() << '\n';
            appendContent(entry.path(), configFile);
        }

        else if(!entry.is_regular_file()) 
            continue;

        if(entry.path().extension() == ".zip")
            configFile << "Zip=" << entry.path().c_str() << '\n';
    }
}
