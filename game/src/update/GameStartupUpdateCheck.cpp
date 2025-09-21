// game/src/util/GameStartupUpdateCheck.cpp
#include "update/GameStartupUpdateCheck.h"

#include <optional>
#include <string>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <shlobj.h>     // SHGetKnownFolderPath
  #include <combaseapi.h> // CoTaskMemFree
#endif

namespace {

// --- config you might want to tweak ---
static constexpr wchar_t kRepo[]        = L"Vidrolll/BeneathTheSurface";
static constexpr wchar_t kAssetRegex[]  = L"BeneathTheSurface-.*\\.zip";
static constexpr wchar_t kGameExeName[] = L"BeneathTheSurface.exe";
static constexpr DWORD   kCheckTimeoutMs = 15000; // give network time to respond
// --------------------------------------

// small helper to quote args for CreateProcess
std::wstring Quote(const std::wstring& s) {
    return L"\"" + s + L"\"";
}

#ifdef _WIN32
// Robust LocalAppData\BeneathTheSurface
std::wstring ResolveInstallDir() {
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
        std::wstring dir = std::wstring(path) + L"\\BeneathTheSurface";
        CoTaskMemFree(path);
        std::error_code ec;
        std::filesystem::create_directories(dir, ec); // ignore errors
        return dir;
    }
    // fallback to env var
    if (wchar_t* env = _wgetenv(L"LOCALAPPDATA")) {
        std::wstring dir = std::wstring(env) + L"\\BeneathTheSurface";
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        return dir;
    }
    // last resort
    return LR"(C:\Users\Public\BeneathTheSurface)";
}

// run a child process, wait up to timeout, return exit code
std::optional<DWORD> RunProcessGetExitCode(const std::wstring& cmd, DWORD timeoutMs) {
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::wstring mutableCmd = cmd; // CreateProcessW may modify this buffer

    if (!CreateProcessW(
            /*app*/ nullptr,
            /*cmd*/ mutableCmd.data(),
            nullptr, nullptr, FALSE,
            CREATE_NO_WINDOW,
            nullptr, nullptr, &si, &pi)) {
        std::wcerr << L"[updater] CreateProcess failed: " << GetLastError() << L"\n";
        return std::nullopt;
    }

    DWORD result = WaitForSingleObject(pi.hProcess, timeoutMs);
    DWORD code = 0;
    if (result == WAIT_OBJECT_0) {
        GetExitCodeProcess(pi.hProcess, &code);
    } else {
        std::wcerr << L"[updater] check-only timed out\n";
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return std::nullopt; // tell caller this wasn't a real "up to date"
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return code;
}
#endif // _WIN32

} // namespace

bool SoftUpdateCheckAndMaybeRun(const std::wstring& installDirArg) {
#ifndef _WIN32
    (void)installDirArg;
    return false; // no-op on non-windows
#else
    // choose caller-supplied dir or resolve default
    const std::wstring installDir = installDirArg.empty() ? ResolveInstallDir() : installDirArg;
    const std::wstring updaterExe = installDir + L"\\updater.exe";

    std::wcout << L"[updater] installDir = " << installDir << L"\n";
    std::wcout << L"[updater] updaterExe = " << updaterExe << L"\n";

    // 1) quick check
    std::wstring checkCmd =
        Quote(updaterExe) + L" --repo " + kRepo +
        L" --install-dir " + Quote(installDir) +
        L" --asset-regex " + Quote(kAssetRegex) +
        L" --check-only";

    auto codeOpt = RunProcessGetExitCode(checkCmd, kCheckTimeoutMs);
    if (!codeOpt.has_value()) {
        std::wcout << L"[updater] no exit code (timeout/launch fail) → continuing launch\n";
        return false;
    }

    DWORD code = *codeOpt; // 11=update available, 10=up-to-date, 2=fail, 0=ok (for full runs)
    std::wcout << L"[updater] check-only exit code = " << code << L"\n";

    if (code != 11) {
        // not updating now
        return false;
    }

    // 2) user accepted (replace with real dialog if you want)
    bool userAccepted = true;
    if (!userAccepted) return false;

    // 3) launch updater for real, ask it to relaunch the game when done
    std::wstring runCmd =
        Quote(updaterExe) + L" --repo " + kRepo +
        L" --install-dir " + Quote(installDir) +
        L" --asset-regex " + Quote(kAssetRegex) +
        L" --version-file " + Quote(installDir + L"\\current.version") +
        L" --relaunch " + Quote(installDir + L"\\" + kGameExeName);

    std::wcout << L"[updater] launching full updater...\n";

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::wstring mutableCmd = runCmd;
    if (!CreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, nullptr, &si, &pi)) {
        std::wcerr << L"[updater] failed to launch updater (CreateProcess): " << GetLastError() << L"\n";
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    // tell caller (main) to exit now so files aren't locked
    return true;
#endif
}
