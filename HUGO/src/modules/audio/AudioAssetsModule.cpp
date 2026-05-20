#include "AudioAssetsModule.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

AudioAssetsModule::AudioAssetsModule(): basePath("") 
{
}

AudioAssetsModule::~AudioAssetsModule() 
{
    clear();
}

std::unique_ptr<AudioAssetsModule> AudioAssetsModule::instanceInternal = std::make_unique<AudioAssetsModule>();

AudioAssetsModule& 
AudioAssetsModule::instance(){
    static AudioAssetsModule& instance = *instanceInternal;
    return instance;
}

void
AudioAssetsModule::destroyInstance() {
    if (!instanceInternal) return;
    instanceInternal.reset();
}

#pragma region PRIVATE HELPERS
bool AudioAssetsModule::isSupportedAudioFile(const HUGO::String& filename) const
{
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) return false;
    
    HUGO::String ext = filename.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    for (const auto& supportedExt : supportedExtensions) {
        if (ext == supportedExt) {
            return true;
        }
    }
    return false;
}

HUGO::String AudioAssetsModule::generateName(const HUGO::String& relativePath) const
{
    // extract only the filename from the path
    HUGO::String filename = relativePath;
    
    // remove any directory path
    size_t slashPos = filename.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        filename = filename.substr(slashPos + 1);
    }
    
    // remove extension
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        filename = filename.substr(0, dotPos);
    }
    
    // convert to lowercase for consistency
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    return filename;
}

void AudioAssetsModule::scanDirectoryRecursive(const std::string& currentPath, const std::string& relativeRoot)
{
    try {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_directory()) {
                // recursively scan subdirectories
                scanDirectoryRecursive(entry.path().string(), relativeRoot);
            } 
            else if (entry.is_regular_file()) {
                HUGO::String filename = entry.path().filename().string();
                
                if (isSupportedAudioFile(filename)) {
                    // get relative path from base directory
                    HUGO::String relativePath = fs::relative(entry.path(), relativeRoot).string();
                    
                    // generate name
                    HUGO::String name = generateName(relativePath);
                    
                    // register audio
                    registerAudio(name, relativePath.c_str());
                    
                    // debug output
                    std::cout << "Registered: " << name 
                              << " -> " << relativePath << "\n";
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "[Filesystem]: " << e.what() << "\n";
    }
}
#pragma endregion

#pragma region PUBLIC API
// Registration
HUGO::AudioID AudioAssetsModule::registerAudio(const HUGO::String& name, const HUGO::String& filepath)
{
    auto it = nameToAsset.find(name);
    if (it != nameToAsset.end()) {
        return it->second->id;
    }

    HUGO::String fullPath = basePath.empty() ? filepath : basePath + "/" + filepath;

    std::shared_ptr<AudioAssetInfo> asset = std::make_shared<AudioAssetInfo>();
    asset->fullPath = fullPath;
    asset->id = nextID++;

    nameToAsset[name] = asset;
    idToName[asset->id] = name;

    return asset->id;
}

// Scan all the paths and store them
size_t AudioAssetsModule::scanAndRegisterAll()
{
    clear();
    
    HUGO::String fullBasePath = basePath.c_str();
    
    if (!fs::exists(fullBasePath)) {
        std::cerr << "Base path does not exist: " << fullBasePath << "\n";
        return 0;
    }
    
    std::cout << "========================================\n";
    std::cout << "Scanning audio assets from: " << fullBasePath << "\n";
    
    scanDirectoryRecursive(fullBasePath, fullBasePath);
    
    std::cout << "========================================\n";
    std::cout << "Registered " << nameToAsset.size() << " audio assets\n";
    std::cout << "========================================\n";
    
    // Auto-save debug file
    saveToDebugFile();
    
    return nameToAsset.size();
}

void AudioAssetsModule::clear()
{
    nameToAsset.clear();
    idToName.clear();
    nextID = 0;
}

// Getters
AudioAssetInfo* AudioAssetsModule::getAsset(const HUGO::String &name) const
{
    auto it = nameToAsset.find(name);
    return it != nameToAsset.end() ? it->second.get() : nullptr;
}

AudioAssetInfo* AudioAssetsModule::getAsset(HUGO::AudioID id) const
{
    auto it = idToName.find(id);
    if (it == idToName.end()) return nullptr;

    auto it2 = nameToAsset.find(it->second);
    return (it2 != nameToAsset.end()) ? it2->second.get() : nullptr;
}

HUGO::AudioID AudioAssetsModule::getID(const HUGO::String &name) const
{
    auto it = nameToAsset.find(name);
    
    if (it != nameToAsset.end()) {
        return it->second->id;
    }
    
    // not found
    return -1;
}

std::vector<HUGO::String> AudioAssetsModule::getAllNames() const
{
    std::vector<HUGO::String> names;
    names.reserve(nameToAsset.size());
    for (const auto& pair : nameToAsset) {
        names.push_back(pair.first);
    }
    return names;
}

// ask if a sound exists
bool AudioAssetsModule::exists(const HUGO::String& name) const
{
    return nameToAsset.find(name) != nameToAsset.end();
}

bool AudioAssetsModule::exists(HUGO::AudioID id) const
{
    return idToName.find(id) != idToName.end();
}

size_t AudioAssetsModule::size() const
{
    return nameToAsset.size();
}

// configuration
void AudioAssetsModule::setBasePath(const HUGO::String& path)
{
    basePath = path;
}

const HUGO::String& AudioAssetsModule::getBasePath() const
{
    return basePath;
}

// debug
bool AudioAssetsModule::saveToDebugFile(const HUGO::String& filename) const
{
    HUGO::String filepath = basePath + filename;
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open debug file: " << filepath << std::endl;
        return false;
    }
    
    file << "AUDIO ASSETS DEBUG\n";
    file << "========================================\n";
    file << "Generated: " << __DATE__ << " " << __TIME__ << "\n";
    file << "Base Path: " << basePath << "\n";
    file << "Total Assets: " << nameToAsset.size() << "\n";
    file << "========================================\n\n";
    
    int index = 1;
    for (const auto& pair : nameToAsset) {
        const HUGO::String& name = pair.first;
        const AudioAssetInfo* info = pair.second.get();
        
        file << index++ << ". " << name << "\n";
        file << "   ID: " << info->id << "\n";
        file << "   Path: " << info->fullPath << "\n";
        
        if (fs::exists(info->fullPath.c_str())) {
            file << "   SUCCESS\n";
        } else {
            file << "   MISS\n";
        }
        
        file << "\n";
    }
    
    file << "========================================\n";
    file << "SUMMARY\n";
    file << "========================================\n";
    
    int missingCount = 0;
    
    for (const auto& pair : nameToAsset) {
        if (!fs::exists(pair.second->fullPath.c_str())) {
            missingCount++;
        }
    }
    
    file << "Total Assets: " << nameToAsset.size() << "\n";
    file << "Files Found: " << (nameToAsset.size() - missingCount) << "\n";
    file << "Files Missing: " << missingCount << "\n";
    file << "========================================\n";
    
    file.close();
    
    std::cout << "Debug file saved: " << filepath << "\n";
    return true;
}
#pragma endregion