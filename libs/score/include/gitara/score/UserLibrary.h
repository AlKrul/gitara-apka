#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace gitara::score {

/// Zwraca true, jeśli nazwa kategorii biblioteki użytkownika jest dozwolona.
[[nodiscard]] bool isAllowedLibraryCategory(std::string_view category) noexcept;

/// Waliduje i zwraca bezpieczne ID elementu biblioteki ([A-Za-z0-9_-], dł. 1..80).
/// Zwraca pusty string dla niepoprawnych identyfikatorów (np. path traversal, kropki, spacje).
[[nodiscard]] std::string sanitizeLibraryId(std::string_view rawId);

/// Tworzy katalog główny biblioteki użytkownika oraz wszystkie dozwolone podkategorie.
[[nodiscard]] bool ensureLibraryDirectories(const std::filesystem::path& rootDir) noexcept;

/// Zapisuje atomowo plik JSON obiektu biblioteki do <rootDir>/<category>/<id>.json.
[[nodiscard]] bool saveLibraryItem(const std::filesystem::path& rootDir,
                                   std::string_view category,
                                   std::string_view id,
                                   std::string_view jsonBody,
                                   std::string* errOut = nullptr);

/// Usuwa element biblioteki <rootDir>/<category>/<id>.json (zwraca true tylko gdy plik istniał i został usunięty).
[[nodiscard]] bool deleteLibraryItem(const std::filesystem::path& rootDir,
                                     std::string_view category,
                                     std::string_view id,
                                     std::string* errOut = nullptr);

/// Skanuje wszystkie podkategorie w <rootDir> i zwraca zbiorczy JSON:
/// {"sweeps":[...],"progressions":[...],"tabs":[...],"studio":[...],"sessions":[...]}.
[[nodiscard]] std::string listAllLibraryItemsJson(const std::filesystem::path& rootDir);

} // namespace gitara::score
