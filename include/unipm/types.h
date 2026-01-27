#pragma once

#include <string>
#include <vector>
#include <map>

namespace unipm {

// Operating system types
enum class OSType {
    LINUX,
    MACOS,
    WINDOWS,
    UNKNOWN
};

// Linux distributions
enum class LinuxDistro {
    UBUNTU,
    DEBIAN,
    ARCH,
    FEDORA,
    RHEL,
    CENTOS,
    OPENSUSE,
    GENTOO,
    UNKNOWN
};

// Package manager types
enum class PackageManager {
    APT,
    PACMAN,
    BREW,
    DNF,
    YUM,
    WINGET,
    CHOCOLATEY,
    SNAP,
    FLATPAK,
    UNKNOWN
};

// Command types
enum class CommandType {
    INSTALL,
    REMOVE,
    UPDATE,
    SEARCH,
    LIST,
    INFO,
    HELP,
    VERSION
};

// OS information structure
struct OSInfo {
    OSType type;
    LinuxDistro distro;
    std::string version;
    std::string codename;
};

// Package manager information
struct PMInfo {
    PackageManager type;
    std::string name;
    std::string path;
    std::string version;
};

// Parsed command structure
struct Command {
    CommandType type;
    std::vector<std::string> packages;
    std::map<std::string, std::string> options;
    bool dryRun = false;
    bool autoYes = false;
    bool verbose = false;
    std::string forcePM;  // Force specific package manager
};

// Package resolution result
struct ResolvedPackage {
    std::string originalName;
    std::string resolvedName;
    std::string version;
    PackageManager packageManager;
    float confidence;  // Matching confidence (0.0 - 1.0)
    std::vector<std::string> suggestions;  // Alternative suggestions
};

// Execution result
struct ExecutionResult {
    bool success;
    int exitCode;
    std::string stdout;
    std::string stderr;
    std::string command;
};

// Helper functions
std::string osTypeToString(OSType type);
std::string linuxDistroToString(LinuxDistro distro);
std::string packageManagerToString(PackageManager pm);
std::string commandTypeToString(CommandType cmd);

PackageManager stringToPackageManager(const std::string& str);

} // namespace unipm
