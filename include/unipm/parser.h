#pragma once

#include "unipm/types.h"
#include <string>
#include <vector>

namespace unipm {

class Parser {
public:
    Parser() = default;
    ~Parser() = default;

    // Parse command-line arguments into a Command structure
    Command parse(int argc, char* argv[]);
    
    // Validate command structure
    bool validate(const Command& cmd, std::string& error);

private:
    CommandType parseCommandType(const std::string& cmd);
    void parseFlags(const std::vector<std::string>& args, Command& cmd, size_t& index);
    std::vector<std::string> parsePackages(const std::vector<std::string>& args, size_t startIndex);
    
    // Helper to check if string is a flag
    bool isFlag(const std::string& arg);
};

} // namespace unipm
