#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gitara::platform {

struct EmbeddedAssetView {
    std::string_view body;
    std::string_view mime;
};

/// Zero-copy reader for the GITAPAK1 asset bundle embedded in the binary.
/// The backing memory buffer passed to parse() must outlive this object.
class EmbeddedAssetPack {
public:
    bool parse(const std::uint8_t* data, std::size_t size);
    void clear() noexcept;

    [[nodiscard]] std::optional<EmbeddedAssetView> find(std::string_view urlPath) const noexcept;
    [[nodiscard]] std::size_t count() const noexcept { return entries_.size(); }
    [[nodiscard]] bool empty() const noexcept { return entries_.empty(); }

private:
    struct Entry {
        std::string_view path;
        std::string_view mime;
        std::string_view body;
    };

    std::vector<Entry> entries_;
};

/// Returns the process-wide embedded asset pack (empty if built without GITARA_EMBEDDED_ASSETS).
[[nodiscard]] const EmbeddedAssetPack& globalEmbeddedAssets();

} // namespace gitara::platform
