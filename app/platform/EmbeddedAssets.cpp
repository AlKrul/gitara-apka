#include "app/platform/EmbeddedAssets.h"

#include <algorithm>
#include <cstring>

namespace gitara::platform {
namespace {

constexpr char kMagic[8] = {'G', 'I', 'T', 'A', 'P', 'A', 'K', '1'};

std::uint32_t readU32Le(const std::uint8_t* p) noexcept {
    return static_cast<std::uint32_t>(p[0]) |
           (static_cast<std::uint32_t>(p[1]) << 8) |
           (static_cast<std::uint32_t>(p[2]) << 16) |
           (static_cast<std::uint32_t>(p[3]) << 24);
}

std::uint64_t readU64Le(const std::uint8_t* p) noexcept {
    std::uint64_t val = 0;
    for (int i = 7; i >= 0; --i) {
        val = (val << 8) | static_cast<std::uint64_t>(p[i]);
    }
    return val;
}

} // namespace

void EmbeddedAssetPack::clear() noexcept {
    entries_.clear();
}

bool EmbeddedAssetPack::parse(const std::uint8_t* data, std::size_t size) {
    clear();
    if (data == nullptr || size < 12) {
        return false;
    }
    if (std::memcmp(data, kMagic, sizeof(kMagic)) != 0) {
        return false;
    }

    const std::uint32_t entryCount = readU32Le(data + 8);
    // Minimum TOC record size is 4 + 4 + 8 + 8 = 24 bytes.
    if (static_cast<std::uint64_t>(entryCount) > (size - 12) / 24) {
        return false;
    }

    std::vector<Entry> parsed;
    parsed.reserve(entryCount);

    std::size_t cursor = 12;
    for (std::uint32_t i = 0; i < entryCount; ++i) {
        if (cursor + 4 > size) return false;
        const std::uint32_t pathLen = readU32Le(data + cursor);
        cursor += 4;
        if (pathLen == 0 || pathLen > size - cursor) return false;
        const std::string_view path(reinterpret_cast<const char*>(data + cursor), pathLen);
        cursor += pathLen;

        if (cursor + 4 > size) return false;
        const std::uint32_t mimeLen = readU32Le(data + cursor);
        cursor += 4;
        if (mimeLen == 0 || mimeLen > size - cursor) return false;
        const std::string_view mime(reinterpret_cast<const char*>(data + cursor), mimeLen);
        cursor += mimeLen;

        if (cursor + 16 > size) return false;
        const std::uint64_t dataOffset = readU64Le(data + cursor);
        const std::uint64_t dataSize = readU64Le(data + cursor + 8);
        cursor += 16;

        if (dataOffset > size || dataSize > size - dataOffset) {
            return false;
        }
        const std::string_view body(
            reinterpret_cast<const char*>(data + static_cast<std::size_t>(dataOffset)),
            static_cast<std::size_t>(dataSize));
        parsed.push_back(Entry{path, mime, body});
    }

    std::sort(parsed.begin(), parsed.end(), [](const Entry& a, const Entry& b) {
        return a.path < b.path;
    });
    entries_ = std::move(parsed);
    return true;
}

std::optional<EmbeddedAssetView> EmbeddedAssetPack::find(std::string_view urlPath) const noexcept {
    if (entries_.empty()) return std::nullopt;
    auto it = std::lower_bound(
        entries_.begin(), entries_.end(), urlPath,
        [](const Entry& e, std::string_view target) {
            return e.path < target;
        });
    if (it != entries_.end() && it->path == urlPath) {
        return EmbeddedAssetView{it->body, it->mime};
    }
    return std::nullopt;
}

} // namespace gitara::platform
