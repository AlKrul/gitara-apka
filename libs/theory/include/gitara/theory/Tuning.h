#pragma once

#include "gitara/theory/Pitch.h"
#include <string>
#include <string_view>
#include <vector>

namespace gitara::theory {

/// Strój gitary (od najniższej struny index=0 do najwyższej index=N-1, np. E2 A2 D3 G3 B3 E4).
struct Tuning {
    std::string id;
    std::string polishName;
    std::vector<Pitch> openStrings;

    [[nodiscard]] int stringCount() const noexcept {
        return static_cast<int>(openStrings.size());
    }
};

/// Zwraca listę wbudowanych strojów gitarowych (Standard, Drop D, Drop C, Open G, Open D, Open E, DADGAD, Eb, D).
[[nodiscard]] const std::vector<Tuning>& builtinTunings();

/// Wyszukuje strój po identyfikatorze (np. "standard", "drop_d", "drop_c", "open_g", "open_d", "dadgad").
[[nodiscard]] const Tuning& findTuning(std::string_view id) noexcept;

} // namespace gitara::theory
