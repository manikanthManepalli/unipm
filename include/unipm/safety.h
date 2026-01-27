#pragma once

#include <string>
#include <vector>

namespace unipm {

class Safety {
public:
    Safety() = default;
    ~Safety() = default;

    // Sanitize input to prevent shell injection
    static std::string sanitize(const std::string& input);
    
    // Validate package name
    static bool isValidPackageName(const std::string& name);
    
    // Validate command
    static bool isValidCommand(const std::string& command);
    
    // Escape shell special characters
    static std::string escapeShell(const std::string& input);
    
    // Log operation to history file
    static void logOperation(const std::string& command, bool success);
    
    // Get history file path
    static std::string getHistoryPath();

private:
    static bool containsDangerousChars(const std::string& input);
};

} // namespace unipm
