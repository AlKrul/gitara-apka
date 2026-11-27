#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace gitara::platform {

struct AutoUpdateOptions {
    std::string currentCommit;
    std::string repo;
    std::string tag;
    bool forceCheck = false;
    std::vector<std::string> forwardArgs;
};

enum class AutoUpdateResult {
    ContinueLaunch,
    UpdatedAndRelaunchReady
};

struct AutoUpdateOutcome {
    AutoUpdateResult result = AutoUpdateResult::ContinueLaunch;
    std::filesystem::path relaunchExePath;
};

void cleanupPreviousUpdateArtifacts();
[[nodiscard]] AutoUpdateOutcome runPreLaunchAutoUpdate(const AutoUpdateOptions& opts);
[[nodiscard]] bool spawnRelaunchedApp(
    const std::filesystem::path& exePath,
    const std::vector<std::string>& forwardArgs);

} // namespace gitara::platform
