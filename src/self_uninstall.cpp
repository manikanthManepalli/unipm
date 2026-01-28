#include "unipm/self_uninstall.h"
#include "unipm/ui.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/stat.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define rmdir _rmdir
    #define unlink _unlink
#else
    #include <unistd.h>
    #include <dirent.h>
#endif

namespace unipm {

void SelfUninstaller::printStep(const std::string& step, bool success) {
    if (success) {
        std::cout << "✓ " << step << std::endl;
    } else {
        std::cout << "✗ " << step << " (failed)" << std::endl;
    }
}

std::string SelfUninstaller::getBinaryPath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::string(buffer);
#else
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::string(buffer);
    }
    return "";
#endif
}

std::string SelfUninstaller::getConfigDirectory() {
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata) {
        return std::string(appdata) + "\\unipm";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.config/unipm";
    }
#endif
    return "";
}

std::string SelfUninstaller::getCacheDirectory() {
#ifdef _WIN32
    const char* localappdata = std::getenv("LOCALAPPDATA");
    if (localappdata) {
        return std::string(localappdata) + "\\unipm\\cache";
    }
#else
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.cache/unipm";
    }
#endif
    return "";
}

bool SelfUninstaller::removeBinary() {
    std::string binaryPath = getBinaryPath();
    
    if (binaryPath.empty()) {
        return false;
    }
    
    // Will be removed by cleanup script
    return true;
}

bool SelfUninstaller::removeConfigDirectory() {
    std::string configDir = getConfigDirectory();
    
    if (configDir.empty()) {
        return true; // Nothing to remove
    }
    
#ifdef _WIN32
    std::string command = "rmdir /s /q \"" + configDir + "\" 2>nul";
#else
    std::string command = "rm -rf '" + configDir + "' 2>/dev/null";
#endif
    
    std::system(command.c_str());
    return true;
}

bool SelfUninstaller::removeCacheDirectory() {
    std::string cacheDir = getCacheDirectory();
    
    if (cacheDir.empty()) {
        return true; // Nothing to remove
    }
    
#ifdef _WIN32
    std::string command = "rmdir /s /q \"" + cacheDir + "\" 2>nul";
#else
    std::string command = "rm -rf '" + cacheDir + "' 2>/dev/null";
#endif
    
    std::system(command.c_str());
    return true;
}

bool SelfUninstaller::removeFromPath() {
#ifdef _WIN32
    // On Windows, we'd need to modify registry
    // This is complex and requires admin rights
    std::cout << "  ℹ Please manually remove unipm from PATH in System Environment Variables" << std::endl;
    return true;
#else
    // On Unix, try to remove from shell configs
    const char* home = std::getenv("HOME");
    if (!home) {
        return false;
    }
    
    std::vector<std::string> shellConfigs = {
        std::string(home) + "/.bashrc",
        std::string(home) + "/.bash_profile",
        std::string(home) + "/.zshrc",
        std::string(home) + "/.profile"
    };
    
    for (const auto& configFile : shellConfigs) {
        std::ifstream inFile(configFile);
        if (!inFile.good()) {
            continue;
        }
        
        std::string content;
        std::string line;
        bool modified = false;
        
        while (std::getline(inFile, line)) {
            // Skip lines that were added by unipm installer
            if (line.find("Added by unipm installer") != std::string::npos) {
                modified = true;
                continue;
            }
            if (line.find("unipm") != std::string::npos && line.find("PATH") != std::string::npos) {
                modified = true;
                continue;
            }
            content += line + "\n";
        }
        inFile.close();
        
        if (modified) {
            std::ofstream outFile(configFile);
            outFile << content;
            outFile.close();
        }
    }
    
    return true;
#endif
}

int SelfUninstaller::uninstall(bool force) {
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << "  unipm uninstall --self" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << std::endl;
    
    if (!force) {
        std::cout << "⚠ This will remove unipm from your system." << std::endl;
        std::cout << std::endl;
        std::cout << "The following will be removed:" << std::endl;
        std::cout << "  • unipm binary: " << getBinaryPath() << std::endl;
        
        std::string configDir = getConfigDirectory();
        if (!configDir.empty()) {
            std::cout << "  • Config directory: " <<configDir << std::endl;
        }
        
        std::string cacheDir = getCacheDirectory();
        if (!cacheDir.empty()) {
            std::cout << "  • Cache directory: " << cacheDir << std::endl;
        }
        
        std::cout << "  • PATH entries (shell configs)" << std::endl;
        std::cout << std::endl;
        
        if (!UI::confirm("Are you sure you want to uninstall unipm?", false)) {
            std::cout << "Uninstallation cancelled." << std::endl;
            return 0;
        }
    }
    
    std::cout << "\nUninstalling unipm..." << std::endl;
    std::cout << std::endl;
    
    bool configRemoved = removeConfigDirectory();
    printStep("Remove config directory", configRemoved);
    
    bool cacheRemoved = removeCacheDirectory();
    printStep("Remove cache directory", cacheRemoved);
    
    bool pathRemoved = removeFromPath();
    printStep("Remove from PATH", pathRemoved);
    
    std::cout << std::endl;
    std::cout << "⚠ The binary will be removed when this process exits." << std::endl;
    std::cout << std::endl;
    std::cout << "✓ unipm has been uninstalled." << std::endl;
    std::cout << std::endl;
    std::cout << "Thanks for using unipm!" << std::endl;
    std::cout << "To reinstall, visit: https://github.com/ieee-cs-bmsit/unipm" << std::endl;
    std::cout << std::endl;
    
    // Schedule binary removal
    std::string binaryPath = getBinaryPath();
    
#ifdef _WIN32
    // On Windows, create a batch script to delete the binary after exit
    std::string scriptPath = std::getenv("TEMP") + std::string("\\unipm-uninstall.bat");
    std::ofstream script(scriptPath);
    script << "@echo off\n";
    script << "timeout /t 2 /nobreak >nul\n";
    script << "del /f /q \"" << binaryPath << "\"\n";
    script << "del /f /q \"%~f0\"\n";
    script.close();
    
    // Execute the script
    std::system(("start /b " + scriptPath).c_str());
#else
    // On Unix, use a simple background process
    std::string command = "sleep 2 && rm -f '" + binaryPath + "' &";
    std::system(command.c_str());
#endif
    
    return 0;
}

} // namespace unipm
