#include "app/platform/AppPaths.h"

#include <cstdlib>
#include <string>
#include <system_error>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <objbase.h>
#include <knownfolders.h>
#include <shlobj.h>
#endif

namespace gitara::platform {
namespace {

#ifdef _WIN32
std::filesystem::path getWindowsEnvPath(const wchar_t* name) {
    DWORD len = ::GetEnvironmentVariableW(name, nullptr, 0);
    if (len == 0) return {};
    std::wstring buf(len, L'\0');
    DWORD written = ::GetEnvironmentVariableW(name, buf.data(), len);
    if (written == 0 || written >= len) return {};
    buf.resize(written);
    if (buf.empty()) return {};
    return std::filesystem::path(buf);
}

std::filesystem::path getKnownFolderPath(REFKNOWNFOLDERID folderId, const wchar_t* fallbackEnv) {
    PWSTR rawPath = nullptr;
    const HRESULT hr = ::SHGetKnownFolderPath(folderId, 0, nullptr, &rawPath);
    if (SUCCEEDED(hr) && rawPath != nullptr) {
        std::filesystem::path p(rawPath);
        ::CoTaskMemFree(rawPath);
        if (!p.empty()) return p;
    } else if (rawPath != nullptr) {
        ::CoTaskMemFree(rawPath);
    }
    return getWindowsEnvPath(fallbackEnv);
}
#else
std::filesystem::path getPosixEnvPath(const char* name) {
    const char* val = std::getenv(name);
    if (val == nullptr || val[0] == '\0') return {};
    return std::filesystem::path(val);
}
#endif

} // namespace

std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    std::vector<wchar_t> buf(512, L'\0');
    for (;;) {
        DWORD copied = ::GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
        if (copied == 0) return {};
        if (copied < buf.size() - 1) {
            return std::filesystem::path(std::wstring(buf.data(), copied));
        }
        if (buf.size() >= 32768) {
            return std::filesystem::path(std::wstring(buf.data(), copied));
        }
        buf.resize(buf.size() * 2, L'\0');
    }
#elif defined(__linux__)
    std::error_code ec;
    auto p = std::filesystem::read_symlink("/proc/self/exe", ec);
    return ec ? std::filesystem::path{} : p;
#else
    return {};
#endif
}

bool isDevCheckoutDir(const std::filesystem::path& cwd) {
    if (cwd.empty()) return false;
    std::error_code ec;
    const bool hasWeb = std::filesystem::exists(cwd / "web" / "js" / "dsp" / "guitarDspEngine.js", ec);
    if (!ec && hasWeb) return true;
    ec.clear();
    const bool hasHtml = std::filesystem::exists(cwd / "app" / "ui" / "DesktopGuiHtml.h", ec);
    return !ec && hasHtml;
}

AppPaths resolveAppPaths(
    bool forcePortable,
    const std::filesystem::path& exePathOverride,
    const std::filesystem::path& cwdOverride) {
    std::error_code ec;
    std::filesystem::path cwd = cwdOverride.empty() ? std::filesystem::current_path(ec) : cwdOverride;
    std::filesystem::path exePath = exePathOverride.empty() ? getExecutablePath() : exePathOverride;
    std::filesystem::path exeDir = exePath.empty() ? cwd : exePath.parent_path();
    if (exeDir.empty()) exeDir = cwd;

    AppPaths out{};
    out.isDevCheckout = isDevCheckoutDir(cwd);

#ifdef _WIN32
    const std::filesystem::path envDataDir = getWindowsEnvPath(L"GITARA_DATA_DIR");
#else
    const std::filesystem::path envDataDir = getPosixEnvPath("GITARA_DATA_DIR");
#endif

    if (!envDataDir.empty()) {
        out.dataDir = envDataDir;
        out.settingsFile = out.dataDir / "settings.json";
        out.userLibraryDir = out.dataDir / "user_library";
        out.runtimeDir = out.dataDir / "runtime";
        out.edgeProfileDir = out.runtimeDir / "EdgeProfile";
        out.activePortFile = out.runtimeDir / "active_port.txt";
        return out;
    }

    if (forcePortable) {
        out.dataDir = exeDir;
        out.settingsFile = out.dataDir / "settings.json";
        out.userLibraryDir = out.dataDir / "user_library";
        out.runtimeDir = out.dataDir / "runtime";
        out.edgeProfileDir = out.runtimeDir / "EdgeProfile";
        out.activePortFile = out.runtimeDir / "active_port.txt";
        return out;
    }

#ifdef _WIN32
    std::filesystem::path roaming = getKnownFolderPath(FOLDERID_RoamingAppData, L"APPDATA");
    if (roaming.empty()) roaming = exeDir;
    std::filesystem::path local = getKnownFolderPath(FOLDERID_LocalAppData, L"LOCALAPPDATA");
    if (local.empty()) local = roaming;

    out.dataDir = roaming / "ApkaGitarowa";
    out.settingsFile = out.dataDir / "settings.json";
    out.userLibraryDir = out.dataDir / "user_library";
    out.runtimeDir = local / "ApkaGitarowa";
    out.edgeProfileDir = out.runtimeDir / "EdgeProfile";
    out.activePortFile = out.runtimeDir / "active_port.txt";
#else
    if (out.isDevCheckout) {
        out.dataDir = cwd;
        out.settingsFile = cwd / ".apka_gitara_settings.json";
        out.userLibraryDir = cwd / "user_library";
        std::filesystem::path tmpBase = std::filesystem::temp_directory_path(ec);
        out.runtimeDir = (ec || tmpBase.empty()) ? (cwd / ".gitara_runtime") : (tmpBase / "apka-gitara-runtime");
        out.edgeProfileDir = out.runtimeDir / "EdgeProfile";
        out.activePortFile = out.runtimeDir / "active_port.txt";
    } else {
        std::filesystem::path xdgData = getPosixEnvPath("XDG_DATA_HOME");
        if (xdgData.empty()) {
            std::filesystem::path home = getPosixEnvPath("HOME");
            if (!home.empty()) {
                xdgData = home / ".local" / "share";
            } else {
                xdgData = exeDir;
            }
        }
        out.dataDir = xdgData / "apka-gitara";
        out.settingsFile = out.dataDir / "settings.json";
        out.userLibraryDir = out.dataDir / "user_library";
        out.runtimeDir = out.dataDir / "runtime";
        out.edgeProfileDir = out.runtimeDir / "EdgeProfile";
        out.activePortFile = out.runtimeDir / "active_port.txt";
    }
#endif

    return out;
}

bool ensureAppDirectories(const AppPaths& paths) noexcept {
    std::error_code ec;
    if (!paths.dataDir.empty()) {
        std::filesystem::create_directories(paths.dataDir, ec);
        if (ec) return false;
    }
    if (!paths.runtimeDir.empty()) {
        std::filesystem::create_directories(paths.runtimeDir, ec);
        if (ec) return false;
    }
    return true;
}

} // namespace gitara::platform
