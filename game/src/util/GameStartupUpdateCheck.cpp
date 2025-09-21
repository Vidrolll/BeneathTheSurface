#include <windows.h>
#include <optional>

#include "util/GameStartupUpdateCheck.h"

static std::optional<DWORD> RunProcessGetExitCode(const std::wstring& cmd, DWORD timeoutMs) {
    STARTUPINFOW si{}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    std::wstring cmdline = cmd;  // CreateProcessW can modify the buffer

    if (!CreateProcessW(nullptr, cmdline.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                        nullptr, nullptr, &si, &pi)) {
        return std::nullopt;
    }
    DWORD res = WaitForSingleObject(pi.hProcess, timeoutMs);
    DWORD code = 0;
    if (res == WAIT_OBJECT_0) {
        GetExitCodeProcess(pi.hProcess, &code);
    } else {
        // Timed out or failed — don’t block startup, just continue
        code = 0;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return code;
}

bool SoftUpdateCheckAndMaybeRun(const std::wstring& installDir) {
    const std::wstring updaterExe = installDir + L"\\updater.exe";
    const std::wstring checkCmd =
        L"\"" + updaterExe + L"\""
        L" --repo Vidrolll/BeneathTheSurface"
        L" --install-dir \"" + installDir + L"\""
        L" --asset-regex \"BeneathTheSurface-.*\\.zip\""
        L" --check-only";

    auto codeOpt = RunProcessGetExitCode(checkCmd, /*timeoutMs*/ 2000);
    if (!codeOpt.has_value()) return false; // couldn’t launch → ignore

    DWORD code = *codeOpt;
    if (code == 11) {
        // Update available → show your UI prompt here
        bool userAccepted = true; // TODO: replace with your actual dialog result
        if (userAccepted) {
            std::wstring runCmd =
                L"\"" + updaterExe + L"\""
                L" --repo Vidrolll/BeneathTheSurface"
                L" --install-dir \"" + installDir + L"\""
                L" --asset-regex \"BeneathTheSurface-.*\\.zip\""
                L" --version-file \"" + installDir + L"\\current.version\""
                L" --relaunch \"" + installDir + L"\\BeneathTheSurface.exe\"";

            // Start updater and exit the game so files aren’t locked
            STARTUPINFOW si{}; si.cb = sizeof(si);
            PROCESS_INFORMATION pi{};
            std::wstring cmd2 = runCmd;
            if (CreateProcessW(nullptr, cmd2.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
            }
            return true; // signal caller to quit now
        }
    }
    // 10 = up-to-date, 0 = OK/no change, 2 = failure → just continue
    return false;
}
