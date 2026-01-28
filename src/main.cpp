#include <iostream>
#include <memory>
#include <vector>

#include "unipm/adapter.h"
#include "unipm/config.h"
#include "unipm/doctor.h"
#include "unipm/executor.h"
#include "unipm/os_detector.h"
#include "unipm/parser.h"
#include "unipm/pm_detector.h"
#include "unipm/resolver.h"
#include "unipm/safety.h"
#include "unipm/self_uninstall.h"
#include "unipm/types.h"
#include "unipm/ui.h"

using namespace unipm;

int main(int argc, char* argv[]) {
    // Parse command-line arguments
    Parser parser;
    Command cmd = parser.parse(argc, argv);
    
    // Handle help and version commands immediately
    if (cmd.type == CommandType::HELP) {
        UI::printHelp();
        return 0;
    }
    
    if (cmd.type == CommandType::VERSION) {
        UI::printVersion();
        return 0;
    }
    
    // Handle doctor command
    if (cmd.type == CommandType::DOCTOR) {
        return Doctor::runDiagnostics();
    }
    
    // Handle self-uninstall command
    if (cmd.type == CommandType::SELF_UNINSTALL) {
        return SelfUninstaller::uninstall(cmd.autoYes);
    }
    
    // Validate command
    std::string error;
    if (!parser.validate(cmd, error)) {
        UI::printError(error);
        UI::printInfo("Use 'unipm help' for usage information");
        return 1;
    }
    
    // Detect operating system
    if (cmd.verbose) {
        UI::printInfo("Detecting operating system...");
    }
    
    OSDetector osDetector;
    OSInfo osInfo = osDetector.detect();
    
    if (cmd.verbose) {
        std::cout << "  OS: " << osTypeToString(osInfo.type);
        if (osInfo.type == OSType::LINUX) {
            std::cout << " (" << linuxDistroToString(osInfo.distro) << ")";
        }
        std::cout << std::endl;
    }
    
    // Detect package managers
    if (cmd.verbose) {
        UI::printInfo("Detecting package managers...");
    }
    
    PMDetector pmDetector;
    PMInfo pmInfo;
    
    // Check if user forced a specific package manager
    if (!cmd.forcePM.empty()) {
        PackageManager forcedPM = stringToPackageManager(cmd.forcePM);
        if (forcedPM == PackageManager::UNKNOWN) {
            UI::printError("Unknown package manager: " + cmd.forcePM);
            return 1;
        }
        
        if (!pmDetector.isAvailable(forcedPM)) {
            UI::printError("Package manager not available: " + cmd.forcePM);
            return 1;
        }
        
        pmInfo.type = forcedPM;
        pmInfo.name = packageManagerToString(forcedPM);
    } else {
        // Auto-detect default package manager
        pmInfo = pmDetector.detectDefault(osInfo);
        
        if (pmInfo.type == PackageManager::UNKNOWN) {
            UI::printError("No package manager found on this system");
            UI::printInfo("You may need to install apt, pacman, brew, dnf, winget, or chocolatey");
            return 1;
        }
    }
    
    if (cmd.verbose) {
        std::cout << "  Using: " << pmInfo.name << std::endl;
    }
    
    // Load configuration and package database
    auto config = std::make_shared<Config>();
    
    // Try to load from installation directory first
    if (!config->loadDefault()) {
        // Fallback: try loading from current directory
        if (!config->load("data/packages.json")) {
            UI::printWarning("Could not load package database");
            UI::printInfo("Using package names as-is without translation");
        }
    }
    
    // Create resolver
    Resolver resolver(config);
    
    // Create adapter for the selected package manager
    auto adapter = AdapterFactory::create(pmInfo.type);
    if (!adapter) {
        UI::printError("Failed to create adapter for " + pmInfo.name);
        return 1;
    }
    
    // Process command
    std::string command;
    std::vector<std::string> resolvedPackages;
    
    switch (cmd.type) {
        case CommandType::INSTALL: {
            // Resolve package names
            for (const auto& pkg : cmd.packages) {
                // Validate package name
                if (!Safety::isValidPackageName(pkg)) {
                    UI::printError("Invalid package name: " + pkg);
                    return 1;
                }
                
                // Split package and version (e.g., "node lts")
                std::string packageName = pkg;
                std::string version;
                
                size_t spacePos = pkg.find(' ');
                if (spacePos != std::string::npos) {
                    packageName = pkg.substr(0, spacePos);
                    version = pkg.substr(spacePos + 1);
                }
                
                // Resolve package
                ResolvedPackage resolved = resolver.resolve(packageName, pmInfo.type, version);
                
                if (cmd.verbose) {
                    UI::printResolution(resolved);
                }
                
                // Warn if confidence is low
                if (resolved.confidence < 0.8f && resolved.confidence > 0.0f) {
                    UI::printWarning("Low confidence match for '" + packageName + "' -> '" + resolved.resolvedName + "'");
                    
                    if (!resolved.suggestions.empty()) {
                        std::cout << "Did you mean:" << std::endl;
                        for (const auto& suggestion : resolved.suggestions) {
                            std::cout << "  - " << suggestion << std::endl;
                        }
                        
                        if (!cmd.autoYes) {
                            if (!UI::confirm("Continue anyway?", false)) {
                                return 0;
                            }
                        }
                    }
                } else if (resolved.confidence == 0.0f) {
                    UI::printWarning("Package '" + packageName + "' not found in database, using as-is");
                }
                
                resolvedPackages.push_back(resolved.resolvedName);
            }
            
            command = adapter->getInstallCommand(resolvedPackages);
            break;
        }
        
        case CommandType::REMOVE: {
            // Resolve package names for removal
            for (const auto& pkg : cmd.packages) {
                ResolvedPackage resolved = resolver.resolve(pkg, pmInfo.type);
                resolvedPackages.push_back(resolved.resolvedName);
            }
            
            command = adapter->getRemoveCommand(resolvedPackages);
            break;
        }
        
        case CommandType::UPDATE: {
            command = adapter->getUpdateCommand();
            break;
        }
        
        case CommandType::SEARCH: {
            if (cmd.packages.empty()) {
                UI::printError("No search query specified");
                return 1;
            }
            command = adapter->getSearchCommand(cmd.packages[0]);
            break;
        }
        
        case CommandType::LIST: {
            command = adapter->getListCommand();
            break;
        }
        
        case CommandType::INFO: {
            if (cmd.packages.empty()) {
                UI::printError("No package specified");
                return 1;
            }
            ResolvedPackage resolved = resolver.resolve(cmd.packages[0], pmInfo.type);
            command = adapter->getInfoCommand(resolved.resolvedName);
            break;
        }
        
        default:
            UI::printError("Unknown command");
            return 1;
    }
    
    // Check if command requires root
    bool requiresRoot = adapter->requiresRoot();
    
    // Dry-run mode
    if (cmd.dryRun) {
        UI::printPreview(command, requiresRoot);
        return 0;
    }
    
    // Confirmation prompt (unless --yes is specified)
    if (!cmd.autoYes && cmd.type == CommandType::INSTALL) {
        std::string packageList;
        for (size_t i = 0; i < resolvedPackages.size(); ++i) {
            if (i > 0) packageList += ", ";
            packageList += resolvedPackages[i];
        }
        
        std::string prompt = "Install " + packageList + " using " + pmInfo.name + "?";
        if (!UI::confirm(prompt, true)) {
            UI::printInfo("Installation cancelled");
            return 0;
        }
    } else if (!cmd.autoYes && cmd.type == CommandType::REMOVE) {
        std::string packageList;
        for (size_t i = 0; i < resolvedPackages.size(); ++i) {
            if (i > 0) packageList += ", ";
            packageList += resolvedPackages[i];
        }
        
        std::string prompt = "Remove " + packageList + " using " + pmInfo.name + "?";
        if (!UI::confirm(prompt, false)) {
            UI::printInfo("Removal cancelled");
            return 0;
        }
    }
    
    // Execute command
    if (!cmd.autoYes) {
        UI::printInfo("Executing: " + command);
    } else {
        UI::printInfo("Installing with auto-confirmation...");
        std::cout << "  Command: " << command << std::endl;
    }
    std::cout << std::endl;  // Add spacing
    
    Executor executor;
    ExecutionResult result = executor.execute(command, requiresRoot);
    
    // Display result - just show success/failure, output already streamed
    std::cout << std::endl;  // Add spacing
    if (result.success) {
        UI::printSuccess("Installation completed successfully");
    } else {
        UI::printError("Installation failed with exit code " + std::to_string(result.exitCode));
    }
    
    return result.success ? 0 : result.exitCode;
}
