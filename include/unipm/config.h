#pragma once

#include "unipm/types.h"
#include "third_party/json.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>

using json = nlohmann::json;

namespace unipm {

struct PackageInfo {
    std::string name;
    std::vector<std::string> aliases;
    std::map<PackageManager, std::string> pmMappings;
    std::map<std::string, std::map<PackageManager, std::string>> versionMappings;
};

class Config {
public:
    Config();
    ~Config() = default;

    // Load package database from JSON file
    bool load(const std::string& path);
    
    // Load default package database
    bool loadDefault();
    
    // Merge user config with default config
    void mergeUserConfig(const std::string& userConfigPath);
    
    // Get package info by name
    PackageInfo getPackageInfo(const std::string& name);
    
    // Check if package exists in database
    bool hasPackage(const std::string& name);
    
    // Get all package names
    std::vector<std::string> getAllPackageNames();
    
    // Get package mapping for specific PM
    std::string getMapping(const std::string& packageName, PackageManager pm);

private:
    json data_;
    std::map<std::string, PackageInfo> packages_;
    
    void parsePackages();
    std::string getDefaultConfigPath();
    std::string getUserConfigPath();
};

} // namespace unipm
