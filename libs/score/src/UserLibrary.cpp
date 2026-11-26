#include "gitara/score/UserLibrary.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <sstream>
#include <system_error>
#include <vector>

namespace gitara::score {
namespace {

constexpr std::array<std::string_view, 5> kCategories = {
    "sweeps",
    "progressions",
    "tabs",
    "studio",
    "sessions"
};

constexpr std::size_t kMaxItemBytes = 3u * 1024u * 1024u;

std::string_view trimAsciiWs(std::string_view s) noexcept {
    while (!s.empty()) {
        unsigned char c = static_cast<unsigned char>(s.front());
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            s.remove_prefix(1);
        } else {
            break;
        }
    }
    while (!s.empty()) {
        unsigned char c = static_cast<unsigned char>(s.back());
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            s.remove_suffix(1);
        } else {
            break;
        }
    }
    return s;
}

bool isValidJsonObjectEnvelope(std::string_view s) noexcept {
    return s.size() >= 2 && s.size() <= kMaxItemBytes && s.front() == '{' && s.back() == '}';
}

} // namespace

bool isAllowedLibraryCategory(std::string_view category) noexcept {
    for (std::string_view c : kCategories) {
        if (c == category) return true;
    }
    return false;
}

std::string sanitizeLibraryId(std::string_view rawId) {
    if (rawId.empty() || rawId.size() > 80) {
        return {};
    }
    for (unsigned char ch : rawId) {
        const bool ok = (ch >= 'a' && ch <= 'z') ||
                        (ch >= 'A' && ch <= 'Z') ||
                        (ch >= '0' && ch <= '9') ||
                        ch == '_' || ch == '-';
        if (!ok) {
            return {};
        }
    }
    return std::string(rawId);
}

bool ensureLibraryDirectories(const std::filesystem::path& rootDir) noexcept {
    std::error_code ec;
    std::filesystem::create_directories(rootDir, ec);
    if (ec) return false;
    for (std::string_view cat : kCategories) {
        std::filesystem::create_directories(rootDir / std::string(cat), ec);
        if (ec) return false;
    }
    return true;
}

bool saveLibraryItem(const std::filesystem::path& rootDir,
                     std::string_view category,
                     std::string_view id,
                     std::string_view jsonBody,
                     std::string* errOut) {
    if (!isAllowedLibraryCategory(category)) {
        if (errOut) *errOut = "Nieobsługiwana kategoria biblioteki.";
        return false;
    }
    const std::string safeId = sanitizeLibraryId(id);
    if (safeId.empty()) {
        if (errOut) *errOut = "Nieprawidłowy identyfikator elementu (dozwolone znaki A-Z, 0-9, _ , -).";
        return false;
    }
    const std::string_view trimmed = trimAsciiWs(jsonBody);
    if (!isValidJsonObjectEnvelope(trimmed)) {
        if (errOut) *errOut = "Zawartość elementu musi być poprawnym obiektem JSON.";
        return false;
    }
    if (!ensureLibraryDirectories(rootDir)) {
        if (errOut) *errOut = "Nie udało się utworzyć katalogów biblioteki.";
        return false;
    }

    const std::filesystem::path catDir = rootDir / std::string(category);
    const std::filesystem::path finalPath = catDir / (safeId + ".json");
    const std::filesystem::path tmpPath = catDir / (safeId + ".json.tmp");

    {
        std::ofstream ofs(tmpPath, std::ios::binary | std::ios::trunc);
        if (!ofs) {
            if (errOut) *errOut = "Nie udało się otworzyć pliku tymczasowego do zapisu.";
            return false;
        }
        ofs.write(trimmed.data(), static_cast<std::streamsize>(trimmed.size()));
        ofs.flush();
        if (!ofs.good()) {
            if (errOut) *errOut = "Błąd zapisu danych do pliku biblioteki.";
            return false;
        }
    }

    std::error_code ec;
    std::filesystem::rename(tmpPath, finalPath, ec);
    if (ec) {
        std::error_code rmEc;
        std::filesystem::remove(tmpPath, rmEc);
        if (errOut) *errOut = "Nie udało się zapisać docelowego pliku biblioteki.";
        return false;
    }
    return true;
}

bool deleteLibraryItem(const std::filesystem::path& rootDir,
                       std::string_view category,
                       std::string_view id,
                       std::string* errOut) {
    if (!isAllowedLibraryCategory(category)) {
        if (errOut) *errOut = "Nieobsługiwana kategoria biblioteki.";
        return false;
    }
    const std::string safeId = sanitizeLibraryId(id);
    if (safeId.empty()) {
        if (errOut) *errOut = "Nieprawidłowy identyfikator elementu.";
        return false;
    }

    const std::filesystem::path target = rootDir / std::string(category) / (safeId + ".json");
    std::error_code ec;
    const bool removed = std::filesystem::remove(target, ec);
    if (ec || !removed) {
        if (errOut) *errOut = "Nie znaleziono elementu do usunięcia.";
        return false;
    }
    return true;
}

std::string listAllLibraryItemsJson(const std::filesystem::path& rootDir) {
    struct Entry {
        std::filesystem::file_time_type mtime{};
        std::string filename;
        std::string jsonBody;
    };

    std::ostringstream oss;
    oss << "{";
    for (std::size_t ci = 0; ci < kCategories.size(); ++ci) {
        if (ci > 0) oss << ",";
        std::string_view cat = kCategories[ci];
        oss << "\"" << cat << "\":[";

        const std::filesystem::path catDir = rootDir / std::string(cat);
        std::error_code ec;
        if (std::filesystem::exists(catDir, ec) && std::filesystem::is_directory(catDir, ec)) {
            std::vector<Entry> entries;
            for (std::filesystem::directory_iterator it(catDir, ec), endIt;
                 !ec && it != endIt;
                 it.increment(ec)) {
                const auto& p = it->path();
                if (!it->is_regular_file(ec) || ec) continue;
                if (p.extension() != ".json") continue;
                const std::string stem = p.stem().string();
                if (sanitizeLibraryId(stem).empty()) continue;

                std::ifstream ifs(p, std::ios::binary);
                if (!ifs) continue;
                std::string raw((std::istreambuf_iterator<char>(ifs)),
                                std::istreambuf_iterator<char>());
                std::string_view trimmed = trimAsciiWs(raw);
                if (!isValidJsonObjectEnvelope(trimmed)) continue;

                std::error_code tmEc;
                auto mtime = std::filesystem::last_write_time(p, tmEc);
                entries.push_back(Entry{
                    tmEc ? std::filesystem::file_time_type{} : mtime,
                    p.filename().string(),
                    std::string(trimmed)
                });
            }

            std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
                if (a.mtime != b.mtime) return a.mtime > b.mtime;
                return a.filename < b.filename;
            });

            for (std::size_t i = 0; i < entries.size(); ++i) {
                if (i > 0) oss << ",";
                oss << entries[i].jsonBody;
            }
        }
        oss << "]";
    }
    oss << "}";
    return oss.str();
}

} // namespace gitara::score
