// src/updater_main.cpp
#include "Updater.h"
#include <iostream>
#include <string>
#include <optional>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

static void printUsage(const char* argv0) {
    std::cout <<
        "Usage:\n"
        "  " << argv0 << " --repo <owner/name> --install-dir <path> "
        "--asset-regex <regex> [--version-file <path>] [--token <gh_token>] "
        "[--check-only] [--relaunch <exe_path>]\n";
}

// Exit codes for soft-check
enum Exit {
    OK = 0,
    USAGE = 1,
    FAIL = 2,
    UP_TO_DATE = 10,
    UPDATE_AVAILABLE = 11
};

int main(int argc, char** argv) {
    std::string repo, installDir, assetRegex, versionFile, relaunchPath;
    std::optional<std::string> token;
    bool checkOnly = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&](std::string& dst){
            if (i + 1 >= argc) { printUsage(argv[0]); return false; }
            dst = argv[++i]; return true;
        };
        if (a == "--repo") { if (!next(repo)) return USAGE; }
        else if (a == "--install-dir") { if (!next(installDir)) return USAGE; }
        else if (a == "--asset-regex") { if (!next(assetRegex)) return USAGE; }
        else if (a == "--version-file") { if (!next(versionFile)) return USAGE; }
        else if (a == "--token") { std::string t; if (!next(t)) return USAGE; token = t; }
        else if (a == "--check-only") { checkOnly = true; }
        else if (a == "--relaunch") { if (!next(relaunchPath)) return USAGE; }
        else if (a == "--help" || a == "-h") { printUsage(argv[0]); return OK; }
        else { std::cerr << "Unknown arg: " << a << "\n"; printUsage(argv[0]); return USAGE; }
    }

    if (repo.empty() || installDir.empty() || assetRegex.empty()) {
        printUsage(argv[0]); return USAGE;
    }
#ifdef _WIN32
    if (versionFile.empty()) versionFile = installDir + "\\current.version";
#else
    if (versionFile.empty()) versionFile = installDir + "/current.version";
#endif

    try {
        Updater u(repo, installDir, versionFile, assetRegex, token);

        if (checkOnly) {
            std::string local, remote;
            auto res = u.checkLatest(&local, &remote);
            if (res == Updater::CheckResult::UpToDate) {
                std::cout << "Up to date: " << (remote.empty() ? "(none)" : remote) << "\n";
                return UP_TO_DATE; // 10
            } else {
                std::cout << "Update available: local='" << (local.empty() ? "(none)" : local)
                          << "' -> remote='" << (remote.empty() ? "(unknown)" : remote) << "'\n";
                return UPDATE_AVAILABLE; // 11
            }
        }


        // Full update path
        bool updated = u.run();
        if (updated) {
            std::cout << "Updated to " << u.getRemoteVersion() << " via asset '" << u.getPickedAssetName() << "'.\n";
#ifdef _WIN32
            if (!relaunchPath.empty()) {
                // Relaunch game
                std::wstring cmd = L"\"" + std::wstring(relaunchPath.begin(), relaunchPath.end()) + L"\"";
                STARTUPINFOW si{}; si.cb = sizeof(si);
                PROCESS_INFORMATION pi{};
                CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
                if (pi.hThread) CloseHandle(pi.hThread);
                if (pi.hProcess) CloseHandle(pi.hProcess);
            }
#endif
        } else {
            std::cout << "Already up to date (" << (u.getLocalVersion().empty() ? "(none)" : u.getLocalVersion()) << ").\n";
        }
        return OK;
    } catch (const std::exception& ex) {
        std::cerr << "Update failed: " << ex.what() << "\n";
        return FAIL; // 2
    }
}
