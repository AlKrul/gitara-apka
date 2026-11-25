#pragma once

#include "gitara/score/Score.h"
#include <string>
#include <string_view>

namespace gitara::score {

/// Parsuje znormalizowany JSON utworu ze Studia TAB (wielościeżkowy, markery, efekty, perkusja)
/// do natywnego modelu gitara::score::Song bez zewnętrznych zależności.
[[nodiscard]] bool parseStudioSongJson(std::string_view jsonText,
                                         Song& outSong,
                                         std::string* outError = nullptr);

} // namespace gitara::score
