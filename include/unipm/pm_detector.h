#pragma once

#include "unipm/types.h"
#include <vector>

namespace unipm {

class PMDetector {
public:
    PMDetector() = default;
    ~PMDetector() = default;

    // Detect all available package managers on the system
    std::vector<PMInfo> detectAll();
    
    // Detect the default/preferred package manager for this OS
    PMInfo detectDefault(const OSInfo& osInfo);
    
    // Check if a specific package manager is available
    bool isAvailable(PackageManager pm);

private:
    // Check if a binary exists in PATH
    bool checkBinary(const std::string& name);
    
    // Get the full path to a binary
    std::string getBinaryPath(const std::string& name);
    
    // Get version of a package manager
    std::string getVersion(PackageManager pm, const std::string& path);
    
    // Detect individual package managers
    PMInfo detectAPT();
    PMInfo detectPacman();
    PMInfo detectBrew();
    PMInfo detectDNF();
    PMInfo detectYUM();
    PMInfo detectWinget();
    PMInfo detectChocolatey();
    PMInfo detectSnap();
    PMInfo detectFlatpak();
};

} // namespace unipm
