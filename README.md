# unipm - Universal Package Manager

![CI Status](https://github.com/ieee-cs-bmsit/unipm/actions/workflows/ci.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)
![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

**unipm** is a cross-platform command-line tool that simplifies software installation with natural commands. It automatically detects your OS and package manager, resolves package names, and executes the correct installation command — all while maintaining transparency and safety.

## Features

- **Unified Syntax**: One command works across all platforms
- **Auto-Detection**: Automatically detects OS, distribution, and package manager
- **Fuzzy Matching**: Intelligent package name resolution with suggestions
- **Safety First**: Dry-run mode, confirmation prompts, and input sanitization
- **Cross-Platform**: Linux, macOS, and Windows support
- **Extensible**: JSON-based package knowledge base

## Quick Start

### Installation

#### From Source

**Linux/macOS:**
```bash
git clone https://github.com/ieee-cs-bmsit/unipm.git
cd unipm
chmod +x scripts/build.sh
./scripts/build.sh
sudo make install -C build
```

**Windows:**
```powershell
git clone https://github.com/ieee-cs-bmsit/unipm.git
cd unipm
.\scripts\build.ps1
# Copy build\bin\Release\unipm.exe to a directory in your PATH
```

### Usage

```bash
# Install packages
unipm install docker
unipm install node lts
unipm install vscode python git

# Remove packages
unipm remove nginx

# Update all packages
unipm update

# Search for packages
unipm search postgres

# Show package info
unipm info docker

# Dry run (preview command)
unipm install redis --dry-run

# Skip confirmation prompts
unipm install mongodb --yes

# Force specific package manager
unipm install docker --pm=brew
```

## Supported Package Managers

- **APT** (Debian, Ubuntu)
- **Pacman** (Arch Linux)
- **Homebrew** (macOS, Linux)
- **DNF** (Fedora, RHEL, CentOS)
- **Winget** (Windows 10/11)
- **Chocolatey** (Windows)

## Use Cases

- **Developer Environment Setup**: Quickly install dev tools across different machines
- **Linux Onboarding**: Help new users without package manager knowledge
- **CI/CD Automation**: Standardize installation scripts across platforms
- **Educational Labs**: Simplify software installation for students

## Examples

### Install Docker on any platform
```bash
unipm install docker
```
- **Ubuntu**: `sudo apt install -y docker.io`
- **Arch**: `sudo pacman -S --noconfirm docker`
- **macOS**: `brew install docker`
- **Windows**: `winget install --id Docker.DockerDesktop`

### Install Node.js LTS
```bash
unipm install node lts
```
- **Homebrew**: `brew install node@lts`
- **Winget**: `winget install --id OpenJS.NodeJS.LTS`

### Preview before installing
```bash
unipm install postgres --dry-run
# Output: Would execute: sudo apt install -y postgresql
```

## Configuration

unipm uses a JSON package database located at:
- **Linux**: `/usr/local/share/unipm/packages.json`
- **macOS**: `/usr/local/share/unipm/packages.json`
- **Windows**: `C:\Program Files\unipm\packages.json`

You can extend the database with custom mappings at:
- **Linux/macOS**: `~/.config/unipm/packages.json`
- **Windows**: `%APPDATA%\unipm\packages.json`

### Example Custom Mapping
```json
{
  "packages": {
    "myapp": {
      "aliases": ["my-application"],
      "apt": "myapp-deb",
      "pacman": "myapp",
      "brew": "myapp-formula"
    }
  }
}
```

## Architecture

unipm follows a modular three-layer architecture:

1. **Parsing Layer**: Command-line argument parsing and validation
2. **Resolution Layer**: Package name fuzzy matching and alias resolution
3. **Execution Layer**: Safe command execution with privilege escalation

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed design documentation.

## Testing

```bash
cd build
ctest
```

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues.

See [CONTRIBUTING.md](.github/CONTRIBUTING.md) for details on how to get started.

### Community & Governance

- **Code of Conduct**: We differ to the [Contributor Covenant](CODE_OF_CONDUCT.md) to ensure a welcoming community.
- **Security**: Please review our [Security Policy](SECURITY.md) for reporting vulnerabilities.


### Adding New Package Mappings

Edit `data/packages.json`:
```json
{
  "newpackage": {
    "aliases": ["alternative-name"],
    "apt": "package-name-for-apt",
    "pacman": "package-name-for-pacman",
    "brew": "package-name-for-brew",
    "dnf": "package-name-for-dnf",
    "winget": "Publisher.PackageName",
    "choco": "package-name"
  }
}
```

### Adding New Package Manager Support

1. Create adapter in `src/adapters/yourpm_adapter.cpp`
2. Implement `PackageManagerAdapter` interface
3. Add to `AdapterFactory::create()`
4. Update `PackageManager` enum in `types.h`

## License

MIT License - see LICENSE file for details

## Acknowledgments

- Inspired by the universal package manager concept
- Uses [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing

## Disclaimer

unipm is a convenience wrapper around native package managers. It does not replace them and requires them to be installed on your system. Always review commands before execution using `--dry-run`.
