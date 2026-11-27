#pragma once

#include <filesystem>

namespace gitara::platform {

struct AppPaths {
    std::filesystem::path dataDir;
    std::filesystem::path settingsFile;
    std::filesystem::path userLibraryDir;
    std::filesystem::path runtimeDir;
    std::filesystem::path edgeProfileDir;
    std::filesystem::path activePortFile;
    bool isDevCheckout = false;
};

[[nodiscard]] std::filesystem::path getExecutablePath();
[[nodiscard]] bool isDevCheckoutDir(const std::filesystem::path& cwd);

[[nodiscard]] AppPaths resolveAppPaths(
    bool forcePortable,
    const std::filesystem::path& exePathOverride = {},
    const std::filesystem::path& cwdOverride = {});

bool ensureAppDirectories(const AppPaths& paths) noexcept;

} // namespace gitara::platform
