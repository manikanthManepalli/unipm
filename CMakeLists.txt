cmake_minimum_required(VERSION 3.15)
project(unipm VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Source files
set(UNIPM_SOURCES
    src/main.cpp
    src/os_detector.cpp
    src/pm_detector.cpp
    src/parser.cpp
    src/resolver.cpp
    src/config.cpp
    src/executor.cpp
    src/safety.cpp
    src/ui.cpp
    src/adapters/apt_adapter.cpp
    src/adapters/pacman_adapter.cpp
    src/adapters/brew_adapter.cpp
    src/adapters/dnf_adapter.cpp
    src/adapters/winget_adapter.cpp
    src/adapters/choco_adapter.cpp
)

# Main executable
add_executable(unipm ${UNIPM_SOURCES})

# Platform-specific libraries
if(WIN32)
    target_link_libraries(unipm PRIVATE ws2_32)
endif()

# Install targets
install(TARGETS unipm DESTINATION bin)
install(FILES data/packages.json DESTINATION share/unipm)

# Testing
enable_testing()
add_subdirectory(tests)
