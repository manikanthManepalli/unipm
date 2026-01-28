#include "unipm/ui.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

namespace unipm {

// ANSI color codes
const std::string UI::RESET = "\033[0m";
const std::string UI::RED = "\033[31m";
const std::string UI::GREEN = "\033[32m";
const std::string UI::YELLOW = "\033[33m";
const std::string UI::BLUE = "\033[34m";
const std::string UI::CYAN = "\033[36m";
const std::string UI::BOLD = "\033[1m";

void UI::printInfo(const std::string& message) {
    std::cout << colorize("ℹ " + message, CYAN) << std::endl;
}

void UI::printSuccess(const std::string& message) {
    std::cout << colorize("✓ " + message, GREEN) << std::endl;
}

void UI::printWarning(const std::string& message) {
    std::cout << colorize("⚠ " + message, YELLOW) << std::endl;
}

void UI::printError(const std::string& message) {
    std::cerr << colorize("✗ " + message, RED) << std::endl;
}

void UI::printPreview(const std::string& command, bool requiresRoot) {
    std::cout << colorize("Preview:", BOLD) << std::endl;
    std::cout << "  " << colorize(command, YELLOW) << std::endl;
    if (requiresRoot) {
        std::cout << colorize("  (requires elevated privileges)", YELLOW) << std::endl;
    }
}

bool UI::confirm(const std::string& message, bool defaultYes) {
    std::string prompt = message + (defaultYes ? " [Y/n]: " : " [y/N]: ");
    std::cout << colorize(prompt, CYAN);
    std::cout.flush();  // Ensure prompt is displayed before reading input
    
    std::string response;
    if (!std::getline(std::cin, response)) {
        // If input fails, return default
        return defaultYes;
    }
    
    if (response.empty()) {
        return defaultYes;
    }
    
    char first = std::tolower(response[0]);
    return first == 'y';
}

void UI::printResolution(const ResolvedPackage& pkg) {
    std::cout << colorize("Package Resolution:", BOLD) << std::endl;
    std::cout << "  Original: " << pkg.originalName << std::endl;
    std::cout << "  Resolved: " << pkg.resolvedName << std::endl;
    std::cout << "  Package Manager: " << packageManagerToString(pkg.packageManager) << std::endl;
    
    if (!pkg.version.empty()) {
        std::cout << "  Version: " << pkg.version << std::endl;
    }
    
    std::cout << "  Confidence: " << (pkg.confidence * 100) << "%" << std::endl;
    
    if (pkg.confidence < 1.0f && !pkg.suggestions.empty()) {
        std::cout << colorize("  Suggestions:", YELLOW) << std::endl;
        for (const auto& suggestion : pkg.suggestions) {
            std::cout << "    - " << suggestion << std::endl;
        }
    }
}

void UI::printHelp() {
    std::cout << colorize("unipm - Universal Package Manager", BOLD) << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  unipm <command> [options] [packages...]" << std::endl;
    std::cout << std::endl;
    std::cout << colorize("Commands:", BOLD) << std::endl;
    std::cout << "  install, i        Install package(s)" << std::endl;
    std::cout << "  remove, rm        Remove package(s)" << std::endl;
    std::cout << "  update, upgrade   Update all packages" << std::endl;
    std::cout << "  search, find      Search for packages" << std::endl;
    std::cout << "  list, ls          List installed packages" << std::endl;
    std::cout << "  info, show        Show package information" << std::endl;
    std::cout << "  doctor            Run system diagnostics" << std::endl;
    std::cout << "  help              Show this help message" << std::endl;
    std::cout << "  version           Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << colorize("Self-Management:", BOLD) << std::endl;
    std::cout << "  uninstall --self  Uninstall unipm from your system" << std::endl;
    std::cout << std::endl;
    std::cout << colorize("Options:", BOLD) << std::endl;
    std::cout << "  --dry-run, -n     Preview command without executing" << std::endl;
    std::cout << "  --yes, -y         Skip confirmation prompts" << std::endl;
    std::cout << "  --verbose, -V     Show detailed output" << std::endl;
    std::cout << "  --pm=<manager>    Force specific package manager" << std::endl;
    std::cout << std::endl;
    std::cout << colorize("Examples:", BOLD) << std::endl;
    std::cout << "  unipm install docker" << std::endl;
    std::cout << "  unipm install node lts" << std::endl;
    std::cout << "  unipm install vscode --yes" << std::endl;
    std::cout << "  unipm remove nginx --dry-run" << std::endl;
    std::cout << "  unipm search postgres" << std::endl;
    std::cout << "  unipm update" << std::endl;
}

void UI::printVersion() {
    std::cout << colorize("unipm version 1.0.0", BOLD) << std::endl;
    std::cout << "Universal Package Manager" << std::endl;
}

void UI::printResult(const ExecutionResult& result) {
    if (result.success) {
        printSuccess("Command executed successfully");
    } else {
        printError("Command failed with exit code " + std::to_string(result.exitCode));
    }
    
    if (!result.stdoutOutput.empty()) {
        std::cout << result.stdoutOutput;
    }
    
    if (!result.stderrOutput.empty() && !result.success) {
        std::cerr << colorize("Error output:", RED) << std::endl;
        std::cerr << result.stderrOutput;
    }
}

bool UI::supportsColor() {
#ifdef _WIN32
    // Enable virtual terminal processing on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) {
        return false;
    }
    
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    return SetConsoleMode(hOut, dwMode) != 0;
#else
    // Check if stdout is a terminal
    return ISATTY(FILENO(stdout)) != 0;
#endif
}

std::string UI::colorize(const std::string& text, const std::string& color) {
    if (!supportsColor()) {
        return text;
    }
    return color + text + RESET;
}

} // namespace unipm
