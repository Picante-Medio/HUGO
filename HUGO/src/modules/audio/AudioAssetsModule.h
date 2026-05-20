#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoError.h"

#include <memory>
#include <vector>
#include <unordered_map>

// complete information for an audio asset
struct AudioAssetInfo {
    HUGO::String fullPath;     // full path for loading
    HUGO::AudioID id;          // unique id
};

// module for managing all audio assets globally
class AudioAssetsModule {
private:
    std::unordered_map<HUGO::String, std::shared_ptr<AudioAssetInfo>> nameToAsset;
    std::unordered_map<HUGO::AudioID, HUGO::String> idToName;
    
    HUGO::String basePath;
    HUGO::AudioID nextID = 0;
    
    // supported audio extensions
    std::vector<HUGO::String> supportedExtensions = {".wav", ".mp3", ".flac"};
    
    // Private helper functions
    bool isSupportedAudioFile(const HUGO::String& filename) const;
    HUGO::String generateName(const HUGO::String& relativePath) const;
    void scanDirectoryRecursive(const std::string& currentPath, const std::string& relativeRoot);
    
    AudioAssetsModule();
    ~AudioAssetsModule();
    
    static std::unique_ptr<AudioAssetsModule> instanceInternal;

public:
    static AudioAssetsModule& instance();
    static void destroyInstance();

    // delete copy constructors
    AudioAssetsModule(const AudioAssetsModule&) = delete;
    AudioAssetsModule& operator=(const AudioAssetsModule&) = delete;
    
    // ========================================================================
    // registry
    // ========================================================================
    
    // register an audio file manually
    HUGO::AudioID registerAudio(const HUGO::String& name, const HUGO::String& filepath);
    
    // scan base directory and register all audio files automatically (type auto-detected)
    size_t scanAndRegisterAll();
    
    // clear all registered assets
    void clear();
    
    // ========================================================================
    // getters
    // ========================================================================
    
    // get asset
    AudioAssetInfo* getAsset(const HUGO::String& name) const;
    AudioAssetInfo* getAsset(HUGO::AudioID id) const;
    // get id by name
    HUGO::AudioID getID(const HUGO::String& name) const;

    // get all registered names
    std::vector<HUGO::String> getAllNames() const;
    
    // check if exists by name
    bool exists(const HUGO::String& name) const;
    
    // check if exists by ID
    bool exists(HUGO::AudioID id) const;
    
    // get number of registered assets
    size_t size() const;
    
    // ========================================================================
    // configuration
    // ========================================================================
    
    // set base path for audio files
    void setBasePath(const HUGO::String& path);
    
    // get base path
    const HUGO::String& getBasePath() const;
    
    // ========================================================================
    // debug
    // ========================================================================
    // save all registered audio assets to a text file for debugging
    bool saveToDebugFile(const HUGO::String& filename = "AudioAssetsDebug.log") const;
};