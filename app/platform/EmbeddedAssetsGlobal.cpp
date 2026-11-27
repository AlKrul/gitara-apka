#include "app/platform/EmbeddedAssets.h"

// Dostep do paczki GITAPAK1 wbudowanej w binarke aplikacji. Ten plik jest kompilowany
// WYLACZNIE do celu aplikacji (nie do biblioteki gitara_platform_core ani testow),
// poniewaz tylko aplikacja jest linkowana z wygenerowanym zasobem / blobem.

#if defined(GITARA_EMBEDDED_ASSETS) && defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <cstdint>

#if defined(GITARA_EMBEDDED_ASSETS) && !defined(_WIN32)
#if defined(GITARA_EMBEDDED_ASSETS_CPP)
extern "C" const std::uint8_t* gitara_assets_pak_data() noexcept;
extern "C" std::size_t gitara_assets_pak_size() noexcept;
#else
extern "C" const std::uint8_t gitara_assets_pak_start[];
extern "C" const std::uint8_t gitara_assets_pak_end[];
#endif
#endif

namespace gitara::platform {

const EmbeddedAssetPack& globalEmbeddedAssets() {
    static const EmbeddedAssetPack pack = []() {
        EmbeddedAssetPack p;
#if defined(GITARA_EMBEDDED_ASSETS)
#if defined(_WIN32)
        // RT_RCDATA == MAKEINTRESOURCE(10); wariant W jawnie, bo bez UNICODE makro jest ANSI.
        if (HRSRC res = ::FindResourceW(nullptr, MAKEINTRESOURCEW(101), MAKEINTRESOURCEW(10))) {
            if (HGLOBAL hData = ::LoadResource(nullptr, res)) {
                const auto* bytes = static_cast<const std::uint8_t*>(::LockResource(hData));
                const DWORD sz = ::SizeofResource(nullptr, res);
                if (bytes != nullptr && sz > 0) {
                    p.parse(bytes, static_cast<std::size_t>(sz));
                }
            }
        }
#elif defined(GITARA_EMBEDDED_ASSETS_CPP)
        const std::uint8_t* bytes = gitara_assets_pak_data();
        const std::size_t sz = gitara_assets_pak_size();
        if (bytes != nullptr && sz > 0) {
            p.parse(bytes, sz);
        }
#else
        const std::uintptr_t startAddr = reinterpret_cast<std::uintptr_t>(gitara_assets_pak_start);
        const std::uintptr_t endAddr = reinterpret_cast<std::uintptr_t>(gitara_assets_pak_end);
        if (endAddr > startAddr) {
            p.parse(gitara_assets_pak_start, static_cast<std::size_t>(endAddr - startAddr));
        }
#endif
#endif
        return p;
    }();
    return pack;
}

} // namespace gitara::platform
