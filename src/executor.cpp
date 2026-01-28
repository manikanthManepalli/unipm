#include "unipm/executor.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <array>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

#include "unipm/safety.h"

namespace unipm {

ExecutionResult Executor::execute(const std::string& command, bool requiresRoot) {
    std::string finalCommand = command;

    // Add sudo if needed (Unix only)
#ifdef _WIN32
    (void)requiresRoot;  // Unused on Windows
#else
    if (requiresRoot && !isAdmin()) {
        if (hasSudo()) {
            finalCommand = prependSudo(command);
        }
    }
#endif

    // Log the operation
    Safety::logOperation(finalCommand, false);

    ExecutionResult result;
    result.command = finalCommand;

#ifdef _WIN32
    result = executeWindows(finalCommand);
#else
    result = executeUnix(finalCommand);
#endif

    // Log success/failure
    Safety::logOperation(finalCommand, result.success);

    return result;
}

void Executor::preview(const std::string& command, bool requiresRoot) {
    std::string finalCommand = command;

#ifdef _WIN32
    (void)requiresRoot;  // Unused on Windows
#else
    if (requiresRoot && !isAdmin() && hasSudo()) {
        finalCommand = prependSudo(command);
    }
#endif

    std::cout << "Would execute: " << finalCommand << std::endl;
}

bool Executor::hasSudo() {
#ifdef _WIN32
    return false;
#else
    return system("which sudo >/dev/null 2>&1") == 0;
#endif
}

bool Executor::isAdmin() {
#ifdef _WIN32
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin == TRUE;
#else
    return geteuid() == 0;
#endif
}

std::string Executor::prependSudo(const std::string& command) {
    return "sudo " + command;
}

ExecutionResult Executor::executeWindows(const std::string& command) {
#ifdef _WIN32
    ExecutionResult result;
    result.success = false;
    result.exitCode = -1;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    HANDLE hStdoutRead, hStdoutWrite;
    HANDLE hStderrRead, hStderrWrite;

    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0) ||
        !CreatePipe(&hStderrRead, &hStderrWrite, &sa, 0)) {
        result.stderrOutput = "Failed to create pipes";
        return result;
    }

    SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStderrRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = hStderrWrite;
    si.hStdOutput = hStdoutWrite;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    std::string cmdLine = "cmd.exe /C " + command;

    if (CreateProcessA(NULL, const_cast<char*>(cmdLine.c_str()), NULL, NULL, TRUE, 0, NULL, NULL,
                       &si, &pi)) {
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrWrite);

        // Read output in real-time while process is running
        char buffer[4096];
        DWORD bytesRead;
        DWORD bytesAvail;
        
        while (true) {
            // Check if process is still running
            DWORD exitCode;
            if (GetExitCodeProcess(pi.hProcess, &exitCode) && exitCode != STILL_ACTIVE) {
                result.exitCode = exitCode;
                result.success = (exitCode == 0);
                break;
            }

            // Read available stdout
            PeekNamedPipe(hStdoutRead, NULL, 0, NULL, &bytesAvail, NULL);
            if (bytesAvail > 0) {
                if (ReadFile(hStdoutRead, buffer, std::min((DWORD)(sizeof(buffer) - 1), bytesAvail), &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    std::cout << buffer << std::flush;
                    result.stdoutOutput += buffer;
                }
            }

            // Read available stderr
            PeekNamedPipe(hStderrRead, NULL, 0, NULL, &bytesAvail, NULL);
            if (bytesAvail > 0) {
                if (ReadFile(hStderrRead, buffer, std::min((DWORD)(sizeof(buffer) - 1), bytesAvail), &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    std::cerr << buffer << std::flush;
                    result.stderrOutput += buffer;
                }
            }

            Sleep(100);  // Small delay to avoid busy-waiting
        }

        // Read any remaining output
        while (ReadFile(hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << buffer << std::flush;
            result.stdoutOutput += buffer;
        }

        while (ReadFile(hStderrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cerr << buffer << std::flush;
            result.stderrOutput += buffer;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        result.stderrOutput = "Failed to create process";
    }

    CloseHandle(hStdoutRead);
    CloseHandle(hStderrRead);

    return result;
#else
    ExecutionResult result;
    result.success = false;
    result.stderrOutput = "Windows execution not available on this platform";
    return result;
#endif
}

ExecutionResult Executor::executeUnix(const std::string& command) {
    ExecutionResult result;
    result.success = false;
    result.exitCode = -1;

    // Execute command and capture output
    std::string fullCommand = command + " 2>&1";
    FILE* pipe = popen(fullCommand.c_str(), "r");

    if (!pipe) {
        result.stderrOutput = "Failed to execute command";
        return result;
    }

    result.stdoutOutput = captureOutput(pipe);
    result.exitCode = pclose(pipe);

    // pclose returns exit status in format that needs WEXITSTATUS macro
#ifndef _WIN32
    if (WIFEXITED(result.exitCode)) {
        result.exitCode = WEXITSTATUS(result.exitCode);
    }
#endif

    result.success = (result.exitCode == 0);

    return result;
}

std::string Executor::captureOutput(FILE* pipe) {
    std::array<char, 256> buffer;
    std::string result;

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    return result;
}

}  // namespace unipm