#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoError.h"

#include <memory>
#include <vector>
#include <unordered_map>

// complete information for an audio asset
struct AudioAssetInfo {
    std::string fullPath;     // full path for loading
    HUGO::AudioID id;          // unique id
};

// module for managing all audio assets globally
class AudioAssetsModule {
private:
    std::unordered_map<std::string, std::shared_ptr<AudioAssetInfo>> nameToAsset;
    std::unordered_map<HUGO::AudioID, std::string> idToName;
    
    std::string basePath;
    HUGO::AudioID nextID = 0;
    
    // supported audio extensions
    std::vector<std::string> supportedExtensions = {".wav", ".mp3", ".flac"};
    
    // Private helper functions
    bool isSupportedAudioFile(const std::string& filename) const;
    std::string generateName(const std::string& relativePath) const;
    void scanDirectoryRecursive(const std::string& currentPath, const std::string& relativeRoot);
    
    AudioAssetsModule();
    static std::unique_ptr<AudioAssetsModule> instanceInternal;

public:
    ~AudioAssetsModule();

    static AudioAssetsModule& instance();
    static void destroyInstance();

    // delete copy constructors
    AudioAssetsModule(const AudioAssetsModule&) = delete;
    AudioAssetsModule& operator=(const AudioAssetsModule&) = delete;
    
    // ========================================================================
    // registry
    // ========================================================================
    
    // register an audio file manually
    HUGO::AudioID registerAudio(const std::string& name, const std::string& filepath);
    
    // scan base directory and register all audio files automatically (type auto-detected)
    size_t scanAndRegisterAll();
    
    // clear all registered assets
    void clear();
    
    // ========================================================================
    // getters
    // ========================================================================
    
    // get asset
    AudioAssetInfo* getAsset(const std::string& name) const;
    AudioAssetInfo* getAsset(HUGO::AudioID id) const;
    // get id by name
    HUGO::AudioID getID(const std::string& name) const;

    // get all registered names
    std::vector<std::string> getAllNames() const;
    
    // check if exists by name
    bool exists(const std::string& name) const;
    
    // check if exists by ID
    bool exists(HUGO::AudioID id) const;
    
    // get number of registered assets
    size_t size() const;
    
    // ========================================================================
    // configuration
    // ========================================================================
    
    // set base path for audio files
    void setBasePath(const std::string& path);
    
    // get base path
    const std::string& getBasePath() const;
    
    // ========================================================================
    // debug
    // ========================================================================
    // save all registered audio assets to a text file for debugging
    bool saveToDebugFile(const std::string& filename = "AudioAssetsDebug.log") const;
};