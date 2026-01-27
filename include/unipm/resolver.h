#pragma once

#include "unipm/types.h"
#include "unipm/config.h"
#include <string>
#include <vector>
#include <memory>

namespace unipm {

class Resolver {
public:
    explicit Resolver(std::shared_ptr<Config> config);
    ~Resolver() = default;

    // Resolve a package name for a specific package manager
    ResolvedPackage resolve(const std::string& packageName, 
                           PackageManager pm,
                           const std::string& version = "");
    
    // Get suggestions for a package name
    std::vector<std::string> getSuggestions(const std::string& packageName, size_t maxResults = 5);

private:
    std::shared_ptr<Config> config_;
    
    // Fuzzy matching using Levenshtein distance
    float fuzzyMatch(const std::string& a, const std::string& b);
    
    // Calculate Levenshtein distance
    int levenshteinDistance(const std::string& s1, const std::string& s2);
    
    // Normalize package name for comparison
    std::string normalize(const std::string& name);
};

} // namespace unipm
