#include "app/platform/UpdateLogic.h"

#include <algorithm>
#include <cctype>
#include <cstring>

namespace gitara::platform {
namespace {

constexpr std::uint32_t kSha256K[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

inline std::uint32_t rotr(std::uint32_t x, std::uint32_t n) noexcept {
    return (x >> n) | (x << (32u - n));
}

std::string toLowerAscii(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (unsigned char c : s) {
        out.push_back(static_cast<char>(std::tolower(c)));
    }
    return out;
}

bool iequals(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

bool istartsWith(std::string_view s, std::string_view prefix) noexcept {
    return s.size() >= prefix.size() && iequals(s.substr(0, prefix.size()), prefix);
}

} // namespace

Sha256::Sha256() noexcept {
    reset();
}

void Sha256::reset() noexcept {
    state_[0] = 0x6a09e667u;
    state_[1] = 0xbb67ae85u;
    state_[2] = 0x3c6ef372u;
    state_[3] = 0xa54ff53au;
    state_[4] = 0x510e527fu;
    state_[5] = 0x9b05688cu;
    state_[6] = 0x1f83d9abu;
    state_[7] = 0x5be0cd19u;
    bitCount_ = 0;
    bufferLen_ = 0;
}

void Sha256::transform(const std::uint8_t block[64]) noexcept {
    std::uint32_t w[64]{};
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<std::uint32_t>(block[i * 4]) << 24) |
               (static_cast<std::uint32_t>(block[i * 4 + 1]) << 16) |
               (static_cast<std::uint32_t>(block[i * 4 + 2]) << 8) |
               static_cast<std::uint32_t>(block[i * 4 + 3]);
    }
    for (int i = 16; i < 64; ++i) {
        const std::uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        const std::uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    std::uint32_t a = state_[0];
    std::uint32_t b = state_[1];
    std::uint32_t c = state_[2];
    std::uint32_t d = state_[3];
    std::uint32_t e = state_[4];
    std::uint32_t f = state_[5];
    std::uint32_t g = state_[6];
    std::uint32_t h = state_[7];

    for (int i = 0; i < 64; ++i) {
        const std::uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        const std::uint32_t ch = (e & f) ^ ((~e) & g);
        const std::uint32_t temp1 = h + s1 + ch + kSha256K[i] + w[i];
        const std::uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        const std::uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        const std::uint32_t temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}

void Sha256::update(const void* data, std::size_t size) noexcept {
    if (data == nullptr || size == 0) return;
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    bitCount_ += static_cast<std::uint64_t>(size) * 8u;

    std::size_t idx = 0;
    if (bufferLen_ > 0) {
        const std::size_t take = std::min(size, 64u - bufferLen_);
        std::memcpy(buffer_ + bufferLen_, bytes, take);
        bufferLen_ += take;
        idx += take;
        if (bufferLen_ == 64) {
            transform(buffer_);
            bufferLen_ = 0;
        }
    }

    while (idx + 64 <= size) {
        transform(bytes + idx);
        idx += 64;
    }

    if (idx < size) {
        bufferLen_ = size - idx;
        std::memcpy(buffer_, bytes + idx, bufferLen_);
    }
}

std::array<std::uint8_t, 32> Sha256::finalBytes() noexcept {
    buffer_[bufferLen_++] = 0x80u;
    if (bufferLen_ > 56) {
        std::memset(buffer_ + bufferLen_, 0, 64 - bufferLen_);
        transform(buffer_);
        bufferLen_ = 0;
    }
    std::memset(buffer_ + bufferLen_, 0, 56 - bufferLen_);
    for (int i = 0; i < 8; ++i) {
        buffer_[56 + i] = static_cast<std::uint8_t>((bitCount_ >> ((7 - i) * 8)) & 0xffu);
    }
    transform(buffer_);

    std::array<std::uint8_t, 32> out{};
    for (int i = 0; i < 8; ++i) {
        out[static_cast<std::size_t>(i * 4)] = static_cast<std::uint8_t>((state_[i] >> 24) & 0xffu);
        out[static_cast<std::size_t>(i * 4 + 1)] = static_cast<std::uint8_t>((state_[i] >> 16) & 0xffu);
        out[static_cast<std::size_t>(i * 4 + 2)] = static_cast<std::uint8_t>((state_[i] >> 8) & 0xffu);
        out[static_cast<std::size_t>(i * 4 + 3)] = static_cast<std::uint8_t>(state_[i] & 0xffu);
    }
    reset();
    return out;
}

std::string Sha256::finalHex() noexcept {
    static constexpr char kHex[] = "0123456789abcdef";
    const auto digest = finalBytes();
    std::string hex;
    hex.reserve(64);
    for (std::uint8_t b : digest) {
        hex.push_back(kHex[(b >> 4) & 0x0f]);
        hex.push_back(kHex[b & 0x0f]);
    }
    return hex;
}

std::string sha256Hex(std::string_view data) {
    Sha256 hasher;
    hasher.update(data.data(), data.size());
    return hasher.finalHex();
}

std::optional<UpdateManifest> parseUpdateManifest(std::string_view json) {
    auto skipWs = [&](std::size_t& p) {
        while (p < json.size() && (json[p] == ' ' || json[p] == '\t' || json[p] == '\r' || json[p] == '\n')) {
            ++p;
        }
    };

    auto matchString = [&](std::size_t p, std::string& out, std::size_t& nextPos) -> bool {
        if (p >= json.size() || json[p] != '"') return false;
        ++p;
        out.clear();
        while (p < json.size()) {
            const char c = json[p++];
            if (c == '\\') {
                if (p >= json.size()) return false;
                const char esc = json[p++];
                if (esc == 'n') out.push_back('\n');
                else if (esc == 'r') out.push_back('\r');
                else if (esc == 't') out.push_back('\t');
                else if (esc == 'b') out.push_back('\b');
                else if (esc == 'f') out.push_back('\f');
                else if (esc == 'u') {
                    if (p + 4 > json.size()) return false;
                    unsigned int cp = 0;
                    for (int i = 0; i < 4; ++i) {
                        const char hc = json[p++];
                        cp <<= 4;
                        if (hc >= '0' && hc <= '9') cp |= static_cast<unsigned int>(hc - '0');
                        else if (hc >= 'a' && hc <= 'f') cp |= static_cast<unsigned int>(hc - 'a' + 10);
                        else if (hc >= 'A' && hc <= 'F') cp |= static_cast<unsigned int>(hc - 'A' + 10);
                        else return false;
                    }
                    if (cp < 0x80u) {
                        out.push_back(static_cast<char>(cp));
                    } else if (cp < 0x800u) {
                        out.push_back(static_cast<char>(0xc0u | (cp >> 6)));
                        out.push_back(static_cast<char>(0x80u | (cp & 0x3fu)));
                    } else {
                        out.push_back(static_cast<char>(0xe0u | (cp >> 12)));
                        out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3fu)));
                        out.push_back(static_cast<char>(0x80u | (cp & 0x3fu)));
                    }
                } else {
                    out.push_back(esc);
                }
            } else if (c == '"') {
                nextPos = p;
                return true;
            } else if (static_cast<unsigned char>(c) < 0x20u) {
                return false;
            } else {
                out.push_back(c);
            }
        }
        return false;
    };

    auto findMatching = [&](std::size_t openPos, char openCh, char closeCh) -> std::size_t {
        if (openPos >= json.size() || json[openPos] != openCh) return std::string_view::npos;
        int depth = 0;
        bool inStr = false;
        for (std::size_t p = openPos; p < json.size(); ++p) {
            const char c = json[p];
            if (inStr) {
                if (c == '\\') {
                    if (p + 1 >= json.size()) return std::string_view::npos;
                    ++p;
                    continue;
                }
                if (c == '"') inStr = false;
                continue;
            }
            if (c == '"') {
                inStr = true;
                continue;
            }
            if (c == openCh) ++depth;
            else if (c == closeCh) {
                --depth;
                if (depth == 0) return p;
            }
        }
        return std::string_view::npos;
    };

    std::size_t p = 0;
    skipWs(p);
    if (p >= json.size() || json[p] != '{') return std::nullopt;
    const std::size_t objEnd = findMatching(p, '{', '}');
    if (objEnd == std::string_view::npos) return std::nullopt;

    std::size_t tail = objEnd + 1;
    skipWs(tail);
    if (tail != json.size()) return std::nullopt;

    UpdateManifest m{};
    bool hasSchema = false;
    bool hasVersion = false;
    bool hasCommit = false;
    bool hasUrl = false;
    bool hasSha = false;

    ++p;
    while (p < objEnd) {
        skipWs(p);
        if (p >= objEnd) break;

        std::string key;
        std::size_t afterKey = p;
        if (!matchString(p, key, afterKey)) return std::nullopt;
        p = afterKey;
        skipWs(p);
        if (p >= objEnd || json[p] != ':') return std::nullopt;
        ++p;
        skipWs(p);
        if (p >= objEnd) return std::nullopt;

        if (json[p] == '"') {
            std::string val;
            std::size_t afterVal = p;
            if (!matchString(p, val, afterVal)) return std::nullopt;
            p = afterVal;
            if (key == "version") { m.version = std::move(val); hasVersion = true; }
            else if (key == "commit") { m.commit = std::move(val); hasCommit = true; }
            else if (key == "shortCommit") { m.shortCommit = std::move(val); }
            else if (key == "builtAt") { m.builtAt = std::move(val); }
            else if (key == "assetName") { m.assetName = std::move(val); }
            else if (key == "downloadUrl") { m.downloadUrl = std::move(val); hasUrl = true; }
            else if (key == "sha256") { m.sha256 = std::move(val); hasSha = true; }
        } else if (json[p] == '{') {
            const std::size_t endNested = findMatching(p, '{', '}');
            if (endNested == std::string_view::npos) return std::nullopt;
            p = endNested + 1;
        } else if (json[p] == '[') {
            const std::size_t endNested = findMatching(p, '[', ']');
            if (endNested == std::string_view::npos) return std::nullopt;
            p = endNested + 1;
        } else {
            const std::size_t valStart = p;
            while (p < objEnd && json[p] != ',' && json[p] != ' ' &&
                   json[p] != '\t' && json[p] != '\r' && json[p] != '\n') {
                ++p;
            }
            const std::string_view tok = json.substr(valStart, p - valStart);
            if (tok.empty()) return std::nullopt;
            if (key == "schema" || key == "size") {
                std::uint64_t num = 0;
                for (char ch : tok) {
                    if (ch < '0' || ch > '9') return std::nullopt;
                    const std::uint64_t digit = static_cast<std::uint64_t>(ch - '0');
                    if (num > (UINT64_MAX - digit) / 10u) return std::nullopt;
                    num = num * 10u + digit;
                }
                if (key == "schema") {
                    if (num > 100000u) return std::nullopt;
                    m.schema = static_cast<int>(num);
                    hasSchema = true;
                } else {
                    m.size = num;
                }
            }
        }

        skipWs(p);
        if (p < objEnd) {
            if (json[p] != ',') return std::nullopt;
            ++p;
            skipWs(p);
            if (p >= objEnd) return std::nullopt; // trailing comma invalid
        }
    }

    if (!hasSchema || m.schema != 1 || !hasVersion || m.version.empty() ||
        !hasCommit || m.commit.empty() || !hasUrl || m.downloadUrl.empty() ||
        !hasSha || m.sha256.empty()) {
        return std::nullopt;
    }

    return m;
}

bool isValidHex(std::string_view s, std::size_t expectedLen) noexcept {
    if (s.size() != expectedLen) return false;
    for (unsigned char c : s) {
        const bool isHex = (c >= '0' && c <= '9') ||
                           (c >= 'a' && c <= 'f') ||
                           (c >= 'A' && c <= 'F');
        if (!isHex) return false;
    }
    return true;
}

bool isAllowedDownloadUrl(std::string_view url, std::string_view repo) {
    constexpr std::string_view kScheme = "https://";
    if (!istartsWith(url, kScheme)) return false;
    for (unsigned char c : url) {
        if (c <= 0x20u || c == 0x7fu) return false;
    }
    const std::string_view rest = url.substr(kScheme.size());
    const std::size_t slash = rest.find('/');
    if (slash == std::string_view::npos || slash == 0) return false;
    const std::string_view authority = rest.substr(0, slash);
    if (authority.find('@') != std::string_view::npos ||
        authority.find(':') != std::string_view::npos) {
        return false;
    }
    const std::string_view path = rest.substr(slash);

    if (iequals(authority, "github.com")) {
        if (repo.empty() || repo.find('/') == std::string_view::npos) return false;
        const std::string expectedPrefix = "/" + std::string(repo) + "/";
        return istartsWith(path, expectedPrefix);
    }
    if (iequals(authority, "objects.githubusercontent.com") ||
        iequals(authority, "release-assets.githubusercontent.com")) {
        return path.size() > 1;
    }
    return false;
}

std::string buildManifestUrl(std::string_view repo, std::string_view tag) {
    return "https://github.com/" + std::string(repo) +
           "/releases/download/" + std::string(tag) + "/manifest.json";
}

UpdateCheckDecision evaluateUpdate(
    std::string_view currentCommit,
    const UpdateManifest& manifest,
    std::string_view repo,
    bool allowDevOverride) {
    if (!allowDevOverride && (currentCommit.empty() || currentCommit == "dev")) {
        return UpdateCheckDecision::SkippedDevBuild;
    }
    if (manifest.schema != 1 ||
        !isValidHex(manifest.commit, 40) ||
        !isValidHex(manifest.sha256, 64) ||
        !isAllowedDownloadUrl(manifest.downloadUrl, repo)) {
        return UpdateCheckDecision::InvalidManifest;
    }
    if (toLowerAscii(currentCommit) == toLowerAscii(manifest.commit)) {
        return UpdateCheckDecision::UpToDate;
    }
    return UpdateCheckDecision::UpdateAvailable;
}

} // namespace gitara::platform
