#pragma once

#include "unipm/types.h"
#include <string>
#include <vector>

namespace unipm {

class UI {
public:
    UI() = default;
    ~UI() = default;

    // Print colorized output
    static void printInfo(const std::string& message);
    static void printSuccess(const std::string& message);
    static void printWarning(const std::string& message);
    static void printError(const std::string& message);
    
    // Print command preview
    static void printPreview(const std::string& command, bool requiresRoot);
    
    // Ask for confirmation
    static bool confirm(const std::string& message, bool defaultYes = true);
    
    // Print package resolution results
    static void printResolution(const ResolvedPackage& pkg);
    
    // Print help message
    static void printHelp();
    
    // Print version
    static void printVersion();
    
    // Print execution result
    static void printResult(const ExecutionResult& result);

private:
    // ANSI color codes
    static const std::string RESET;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string CYAN;
    static const std::string BOLD;
    
    // Check if terminal supports colors
    static bool supportsColor();
    
    // Colorize text
    static std::string colorize(const std::string& text, const std::string& color);
};

} // namespace unipm
