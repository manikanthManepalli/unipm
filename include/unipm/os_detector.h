#pragma once

#include "unipm/types.h"
#include <memory>

namespace unipm {

class OSDetector {
public:
    OSDetector() = default;
    ~OSDetector() = default;

    // Detect the current operating system and distribution
    OSInfo detect();

private:
    OSInfo detectLinux();
    OSInfo detectMacOS();
    OSInfo detectWindows();
    
    // Parse /etc/os-release for Linux distro info
    OSInfo parseOSRelease();
    
    // Fallback detection methods
    OSInfo detectFromLSBRelease();
    OSInfo detectFromUname();
};

} // namespace unipm
