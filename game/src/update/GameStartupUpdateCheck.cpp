//
// Created by Caden Vize on 9/21/2025.
//

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
    constexpr wchar_t kRepo[]        = L"Vidrolll/BeneathTheSurface";
    constexpr wchar_t kAssetRegex[]  = L"BeneathTheSurface-.*\\.zip";
    constexpr wchar_t kGameExeName[] = L"BeneathTheSurface.exe";
    constexpr DWORD   kCheckTimeoutMs = 15000;

std::wstring Quote(const std::wstring& s) {
    return L"\"" + s + L"\"";
}

#ifdef _WIN32
std::wstring ResolveInstallDir() {
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
        std::wstring dir = std::wstring(path) + L"\\BeneathTheSurface";
        CoTaskMemFree(path);
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        return dir;
    }
    if (wchar_t* env = _wgetenv(L"LOCALAPPDATA")) {
        std::wstring dir = std::wstring(env) + L"\\BeneathTheSurface";
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        return dir;
    }
    return LR"(C:\Users\Public\BeneathTheSurface)";
}

std::optional<DWORD> RunProcessGetExitCode(const std::wstring& cmd, DWORD timeoutMs) {
    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};
    std::wstring mutableCmd = cmd;

    if (!CreateProcessW(
            nullptr,
            mutableCmd.data(),
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
        return std::nullopt;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return code;
}
#endif

}

bool SoftUpdateCheckAndMaybeRun(const std::wstring& installDirArg) {
#ifndef _WIN32
    (void)installDirArg;
    return false;
#else
    const std::wstring installDir = installDirArg.empty() ? ResolveInstallDir() : installDirArg;
    const std::wstring updaterExe = installDir + L"\\updater.exe";

    std::wcout << L"[updater] installDir = " << installDir << L"\n";
    std::wcout << L"[updater] updaterExe = " << updaterExe << L"\n";

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

    DWORD code = *codeOpt;
    std::wcout << L"[updater] check-only exit code = " << code << L"\n";

    if (code != 11)
        return false;

    bool userAccepted = true;
    if (!userAccepted) return false;

    std::wstring runCmd =
        Quote(updaterExe) + L" --repo " + kRepo +
        L" --install-dir " + Quote(installDir) +
        L" --asset-regex " + Quote(kAssetRegex) +
        L" --version-file " + Quote(installDir + L"\\current.version") +
        L" --relaunch " + Quote(installDir + L"\\" + kGameExeName);

    std::wcout << L"[updater] launching full updater...\n";

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi{};
    std::wstring mutableCmd = runCmd;
    if (!CreateProcessW(nullptr, mutableCmd.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                        nullptr, nullptr, &si, &pi)) {
        std::wcerr << L"[updater] failed to launch updater (CreateProcess): " << GetLastError() << L"\n";
        return false;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return true;
#endif
}
