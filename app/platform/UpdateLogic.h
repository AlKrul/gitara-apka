#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace gitara::platform {

class Sha256 {
public:
    Sha256() noexcept;

    void reset() noexcept;
    void update(const void* data, std::size_t size) noexcept;
    [[nodiscard]] std::array<std::uint8_t, 32> finalBytes() noexcept;
    [[nodiscard]] std::string finalHex() noexcept;

private:
    void transform(const std::uint8_t block[64]) noexcept;

    std::uint32_t state_[8]{};
    std::uint64_t bitCount_{0};
    std::uint8_t buffer_[64]{};
    std::size_t bufferLen_{0};
};

[[nodiscard]] std::string sha256Hex(std::string_view data);

struct UpdateManifest {
    int schema = 0;
    std::string version;
    std::string commit;
    std::string shortCommit;
    std::string builtAt;
    std::string assetName;
    std::string downloadUrl;
    std::string sha256;
    std::uint64_t size = 0;
};

[[nodiscard]] std::optional<UpdateManifest> parseUpdateManifest(std::string_view json);
[[nodiscard]] bool isValidHex(std::string_view s, std::size_t expectedLen) noexcept;
[[nodiscard]] bool isAllowedDownloadUrl(std::string_view url, std::string_view repo);
[[nodiscard]] std::string buildManifestUrl(std::string_view repo, std::string_view tag);

enum class UpdateCheckDecision {
    UpToDate,
    UpdateAvailable,
    SkippedDevBuild,
    InvalidManifest
};

[[nodiscard]] UpdateCheckDecision evaluateUpdate(
    std::string_view currentCommit,
    const UpdateManifest& manifest,
    std::string_view repo,
    bool allowDevOverride = false);

} // namespace gitara::platform
