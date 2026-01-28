#include "unipm/parser.h"
#include <iostream>
#include <algorithm>

namespace unipm {

Command Parser::parse(int argc, char* argv[]) {
    Command cmd;
    
    if (argc < 2) {
        cmd.type = CommandType::HELP;
        return cmd;
    }
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    // Check for "uninstall --self" command
    if (args[0] == "uninstall" && args.size() > 1 && args[1] == "--self") {
        cmd.type = CommandType::SELF_UNINSTALL;
        return cmd;
    }
    
    // First argument is the command
    cmd.type = parseCommandType(args[0]);
    
    if (cmd.type == CommandType::HELP || cmd.type == CommandType::VERSION || 
        cmd.type == CommandType::DOCTOR) {
        return cmd;
    }
    
    // Parse flags and packages
    size_t index = 1;
    parseFlags(args, cmd, index);
    
    // Remaining arguments are package names
    cmd.packages = parsePackages(args, index);
    
    return cmd;
}

bool Parser::validate(const Command& cmd, std::string& error) {
    switch (cmd.type) {
        case CommandType::INSTALL:
        case CommandType::REMOVE:
        case CommandType::SEARCH:
        case CommandType::INFO:
            if (cmd.packages.empty()) {
                error = "No package specified for " + commandTypeToString(cmd.type);
                return false;
            }
            break;
        case CommandType::UPDATE:
        case CommandType::LIST:
        case CommandType::HELP:
        case CommandType::VERSION:
        case CommandType::DOCTOR:
        case CommandType::SELF_UNINSTALL:
            // These don't require packages
            break;
    }
    
    return true;
}

CommandType Parser::parseCommandType(const std::string& cmd) {
    std::string lower = cmd;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "install" || lower == "i") return CommandType::INSTALL;
    if (lower == "remove" || lower == "rm" || lower == "uninstall") return CommandType::REMOVE;
    if (lower == "update" || lower == "upgrade") return CommandType::UPDATE;
    if (lower == "search" || lower == "find") return CommandType::SEARCH;
    if (lower == "list" || lower == "ls") return CommandType::LIST;
    if (lower == "info" || lower == "show") return CommandType::INFO;
    if (lower == "help" || lower == "--help" || lower == "-h") return CommandType::HELP;
    if (lower == "version" || lower == "--version" || lower == "-v") return CommandType::VERSION;
    if (lower == "doctor" || lower == "dr") return CommandType::DOCTOR;
    
    return CommandType::HELP;
}

void Parser::parseFlags(const std::vector<std::string>& args, Command& cmd, size_t& index) {
    while (index < args.size() && isFlag(args[index])) {
        std::string flag = args[index];
        
        if (flag == "--dry-run" || flag == "-n") {
            cmd.dryRun = true;
        } else if (flag == "--yes" || flag == "-y") {
            cmd.autoYes = true;
        } else if (flag == "--verbose" || flag == "-V") {
            cmd.verbose = true;
        } else if (flag.find("--pm=") == 0) {
            cmd.forcePM = flag.substr(5);
        } else if (flag == "--pm" && index + 1 < args.size()) {
            index++;
            cmd.forcePM = args[index];
        }
        
        index++;
    }
}

std::vector<std::string> Parser::parsePackages(const std::vector<std::string>& args, size_t startIndex) {
    std::vector<std::string> packages;
    for (size_t i = startIndex; i < args.size(); ++i) {
        if (!isFlag(args[i])) {
            packages.push_back(args[i]);
        }
    }
    return packages;
}

bool Parser::isFlag(const std::string& arg) {
    return arg.size() > 0 && arg[0] == '-';
}

} // namespace unipm
