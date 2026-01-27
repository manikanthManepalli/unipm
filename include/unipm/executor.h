#pragma once

#include "unipm/types.h"
#include <string>

namespace unipm {

class Executor {
public:
    Executor() = default;
    ~Executor() = default;

    // Execute a command
    ExecutionResult execute(const std::string& command, bool requiresRoot = false);
    
    // Execute with dry-run mode (just print the command)
    void preview(const std::string& command, bool requiresRoot = false);
    
    // Check if sudo is available
    bool hasSudo();
    
    // Check if running with admin privileges
    bool isAdmin();

private:
    std::string prependSudo(const std::string& command);
    ExecutionResult executeWindows(const std::string& command);
    ExecutionResult executeUnix(const std::string& command);
    
    // Capture output from command
    std::string captureOutput(FILE* pipe);
};

} // namespace unipm
