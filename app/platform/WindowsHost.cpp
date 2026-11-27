#include "app/platform/WindowsHost.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shellapi.h>

#include <cstdio>
#include <filesystem>
#include <ios>
#include <string>
#include <system_error>
#include <vector>

namespace gitara::platform {
namespace {

std::wstring getEnvWide(const wchar_t* name) {
    const DWORD len = ::GetEnvironmentVariableW(name, nullptr, 0);
    if (len == 0) return {};
    std::wstring buf(len, L'\0');
    const DWORD written = ::GetEnvironmentVariableW(name, buf.data(), len);
    if (written == 0 || written >= len) return {};
    buf.resize(written);
    return buf;
}

std::filesystem::path queryAppPathFromRegistry(HKEY rootKey, const wchar_t* subKey) {
    wchar_t buf[1024]{};
    DWORD cbData = sizeof(buf);
    const LSTATUS st = ::RegGetValueW(
        rootKey,
        subKey,
        nullptr,
        RRF_RT_REG_SZ,
        nullptr,
        buf,
        &cbData);
    if (st != ERROR_SUCCESS || cbData < sizeof(wchar_t)) return {};

    std::wstring raw(buf);
    if (raw.size() >= 2 && raw.front() == L'"' && raw.back() == L'"') {
        raw = raw.substr(1, raw.size() - 2);
    }
    if (raw.empty()) return {};
    std::filesystem::path p(raw);
    std::error_code ec;
    return std::filesystem::exists(p, ec) ? p : std::filesystem::path{};
}

std::filesystem::path findChromiumAppBrowser() {
    const std::wstring pf86 = getEnvWide(L"ProgramFiles(x86)");
    const std::wstring pf = getEnvWide(L"ProgramFiles");
    const std::wstring localApp = getEnvWide(L"LOCALAPPDATA");

    const std::vector<std::filesystem::path> edgeCandidates = {
        pf86.empty() ? std::filesystem::path{} : std::filesystem::path(pf86) / L"Microsoft\\Edge\\Application\\msedge.exe",
        pf.empty() ? std::filesystem::path{} : std::filesystem::path(pf) / L"Microsoft\\Edge\\Application\\msedge.exe",
        localApp.empty() ? std::filesystem::path{} : std::filesystem::path(localApp) / L"Microsoft\\Edge\\Application\\msedge.exe",
    };

    std::error_code ec;
    for (const auto& cand : edgeCandidates) {
        if (!cand.empty() && std::filesystem::exists(cand, ec)) return cand;
        ec.clear();
    }
    if (auto reg = queryAppPathFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe"); !reg.empty()) {
        return reg;
    }
    if (auto reg = queryAppPathFromRegistry(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\msedge.exe"); !reg.empty()) {
        return reg;
    }

    const std::vector<std::filesystem::path> chromeCandidates = {
        pf.empty() ? std::filesystem::path{} : std::filesystem::path(pf) / L"Google\\Chrome\\Application\\chrome.exe",
        pf86.empty() ? std::filesystem::path{} : std::filesystem::path(pf86) / L"Google\\Chrome\\Application\\chrome.exe",
        localApp.empty() ? std::filesystem::path{} : std::filesystem::path(localApp) / L"Google\\Chrome\\Application\\chrome.exe",
    };
    for (const auto& cand : chromeCandidates) {
        if (!cand.empty() && std::filesystem::exists(cand, ec)) return cand;
        ec.clear();
    }
    if (auto reg = queryAppPathFromRegistry(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\chrome.exe"); !reg.empty()) {
        return reg;
    }
    return {};
}

} // namespace

SingleInstanceLock::~SingleInstanceLock() {
    release();
}

bool SingleInstanceLock::acquire(int retryMs) {
    release();
    constexpr wchar_t kMutexName[] = L"Local\\ApkaGitarowa_SingleInstance_v1";
    const ULONGLONG deadline = ::GetTickCount64() + static_cast<ULONGLONG>(retryMs > 0 ? retryMs : 0);

    for (;;) {
        HANDLE h = ::CreateMutexW(nullptr, FALSE, kMutexName);
        if (h == nullptr) {
            return true;
        }
        if (::GetLastError() != ERROR_ALREADY_EXISTS) {
            mutex_ = h;
            return true;
        }
        ::CloseHandle(h);
        if (retryMs <= 0 || ::GetTickCount64() >= deadline) {
            return false;
        }
        ::Sleep(100);
    }
}

void SingleInstanceLock::release() noexcept {
    if (mutex_ != nullptr) {
        ::CloseHandle(static_cast<HANDLE>(mutex_));
        mutex_ = nullptr;
    }
}

BrowserLaunchResult launchAppWindow(int port, const AppPaths& paths) {
    BrowserLaunchResult result{};
    const std::wstring url = L"http://127.0.0.1:" + std::to_wstring(port);
    const std::filesystem::path browserExe = findChromiumAppBrowser();

    if (!browserExe.empty()) {
        std::error_code ec;
        if (!paths.edgeProfileDir.empty()) {
            std::filesystem::create_directories(paths.edgeProfileDir, ec);
        }

        std::wstring cmdLine = L"\"" + browserExe.wstring() + L"\" --app=" + url;
        if (!paths.edgeProfileDir.empty()) {
            cmdLine += L" --user-data-dir=\"" + paths.edgeProfileDir.wstring() + L"\"";
        }
        cmdLine += L" --no-first-run --no-default-browser-check --window-size=1360,860";

        std::vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
        cmdBuf.push_back(L'\0');

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        if (::CreateProcessW(
                browserExe.c_str(),
                cmdBuf.data(),
                nullptr,
                nullptr,
                FALSE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi)) {
            ::CloseHandle(pi.hThread);
            result.launched = true;
            result.dedicatedProcess = true;
            result.nativeHandle = reinterpret_cast<std::uintptr_t>(pi.hProcess);
            return result;
        }
    }

    const HINSTANCE execRes = ::ShellExecuteW(
        nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<std::uintptr_t>(execRes) > 32) {
        result.launched = true;
        result.dedicatedProcess = false;
    }
    return result;
}

bool isBrowserProcessExited(const BrowserLaunchResult& res) noexcept {
    if (!res.dedicatedProcess || res.nativeHandle == 0) {
        return false;
    }
    HANDLE h = reinterpret_cast<HANDLE>(res.nativeHandle);
    return ::WaitForSingleObject(h, 0) == WAIT_OBJECT_0;
}

void closeBrowserHandle(BrowserLaunchResult& res) noexcept {
    if (res.nativeHandle != 0) {
        ::CloseHandle(reinterpret_cast<HANDLE>(res.nativeHandle));
        res.nativeHandle = 0;
    }
    res.dedicatedProcess = false;
}

void attachConsoleIfPresent() noexcept {
    if (::AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* fOut = nullptr;
        FILE* fErr = nullptr;
        ::freopen_s(&fOut, "CONOUT$", "w", stdout);
        ::freopen_s(&fErr, "CONOUT$", "w", stderr);
        std::ios::sync_with_stdio();
    }
}

} // namespace gitara::platform

#else

#include <cstdlib>
#include <string>

namespace gitara::platform {

SingleInstanceLock::~SingleInstanceLock() = default;

bool SingleInstanceLock::acquire(int retryMs) {
    (void)retryMs;
    return true;
}

void SingleInstanceLock::release() noexcept {}

BrowserLaunchResult launchAppWindow(int port, const AppPaths& paths) {
    (void)paths;
    const std::string url = "http://127.0.0.1:" + std::to_string(port);
    const std::string cmd =
        "( \"/mnt/c/Program Files (x86)/Microsoft/Edge/Application/msedge.exe\" --app=" + url +
        " >/dev/null 2>&1 || cmd.exe /c start " + url +
        " >/dev/null 2>&1 || xdg-open " + url + " >/dev/null 2>&1 ) &";
    const int rc = std::system(cmd.c_str());
    (void)rc;
    return BrowserLaunchResult{true, false, 0};
}

bool isBrowserProcessExited(const BrowserLaunchResult& res) noexcept {
    (void)res;
    return false;
}

void closeBrowserHandle(BrowserLaunchResult& res) noexcept {
    res.nativeHandle = 0;
    res.dedicatedProcess = false;
}

void attachConsoleIfPresent() noexcept {}

} // namespace gitara::platform

#endif
