#pragma once

#include "unipm/types.h"
#include <string>
#include <vector>
#include <memory>

namespace unipm {

// Abstract package manager adapter interface
class PackageManagerAdapter {
public:
    virtual ~PackageManagerAdapter() = default;

    virtual PackageManager getType() const = 0;
    virtual std::string getName() const = 0;
    
    // Generate command strings
    virtual std::string getInstallCommand(const std::vector<std::string>& packages) = 0;
    virtual std::string getRemoveCommand(const std::vector<std::string>& packages) = 0;
    virtual std::string getUpdateCommand() = 0;
    virtual std::string getSearchCommand(const std::string& query) = 0;
    virtual std::string getListCommand() = 0;
    virtual std::string getInfoCommand(const std::string& package) = 0;
    
    // Does this PM require root/admin privileges?
    virtual bool requiresRoot() = 0;
    
    // Format package names for this PM
    virtual std::string formatPackageName(const std::string& name) {
        return name;
    }
};

// Factory for creating adapters
class AdapterFactory {
public:
    static std::unique_ptr<PackageManagerAdapter> create(PackageManager pm);
};

// Concrete adapter implementations

class APTAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::APT; }
    std::string getName() const override { return "apt"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return true; }
};

class PacmanAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::PACMAN; }
    std::string getName() const override { return "pacman"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return true; }
};

class BrewAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::BREW; }
    std::string getName() const override { return "brew"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return false; }
};

class DNFAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::DNF; }
    std::string getName() const override { return "dnf"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return true; }
};

class WingetAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::WINGET; }
    std::string getName() const override { return "winget"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return false; }
};

class ChocolateyAdapter : public PackageManagerAdapter {
public:
    PackageManager getType() const override { return PackageManager::CHOCOLATEY; }
    std::string getName() const override { return "choco"; }
    std::string getInstallCommand(const std::vector<std::string>& packages) override;
    std::string getRemoveCommand(const std::vector<std::string>& packages) override;
    std::string getUpdateCommand() override;
    std::string getSearchCommand(const std::string& query) override;
    std::string getListCommand() override;
    std::string getInfoCommand(const std::string& package) override;
    bool requiresRoot() override { return true; }
};

} // namespace unipm
