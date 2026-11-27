#include "app/platform/AutoUpdater.h"

#include "app/platform/AppPaths.h"
#include "app/platform/UpdateLogic.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <mutex>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

namespace gitara::platform {
namespace {

constexpr UINT WM_GITARA_PHASE_CHANGED = WM_APP + 1;
constexpr UINT WM_GITARA_WORKER_DONE = WM_APP + 2;
constexpr UINT_PTR kTimerShowSplash = 1;
constexpr UINT_PTR kTimerAnimate = 2;

constexpr int kSplashWidth = 420;
constexpr int kSplashHeight = 156;
constexpr RECT kSkipBtnRect{322, 112, 400, 138};

std::wstring utf8ToWide(std::string_view s) {
    if (s.empty()) return {};
    const int needed = ::MultiByteToWideChar(
        CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    if (needed <= 0) return {};
    std::wstring out(static_cast<std::size_t>(needed), L'\0');
    ::MultiByteToWideChar(
        CP_UTF8, 0, s.data(), static_cast<int>(s.size()), out.data(), needed);
    return out;
}

bool iequalsAscii(std::string_view a, std::string_view b) noexcept {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

std::wstring quoteWinArg(const std::wstring& arg) {
    if (!arg.empty() && arg.find_first_of(L" \t\n\v\"") == std::wstring::npos) {
        return arg;
    }
    std::wstring out = L"\"";
    for (auto it = arg.begin();; ++it) {
        std::size_t numBackslashes = 0;
        while (it != arg.end() && *it == L'\\') {
            ++it;
            ++numBackslashes;
        }
        if (it == arg.end()) {
            out.append(numBackslashes * 2, L'\\');
            break;
        }
        if (*it == L'"') {
            out.append(numBackslashes * 2 + 1, L'\\');
            out.push_back(L'"');
        } else {
            out.append(numBackslashes, L'\\');
            out.push_back(*it);
        }
    }
    out.push_back(L'"');
    return out;
}

struct WinHttpHandle {
    HINTERNET handle = nullptr;
    WinHttpHandle() = default;
    explicit WinHttpHandle(HINTERNET h) noexcept : handle(h) {}
    ~WinHttpHandle() {
        if (handle != nullptr) {
            ::WinHttpCloseHandle(handle);
        }
    }
    WinHttpHandle(const WinHttpHandle&) = delete;
    WinHttpHandle& operator=(const WinHttpHandle&) = delete;
    [[nodiscard]] bool valid() const noexcept { return handle != nullptr; }
};

struct ParsedHttpsUrl {
    std::wstring host;
    INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;
    std::wstring pathAndQuery;
};

bool crackHttpsUrl(const std::wstring& url, ParsedHttpsUrl& out) {
    URL_COMPONENTS uc{};
    uc.dwStructSize = sizeof(uc);
    wchar_t hostBuf[512]{};
    wchar_t pathBuf[4096]{};
    wchar_t extraBuf[2048]{};
    uc.lpszHostName = hostBuf;
    uc.dwHostNameLength = static_cast<DWORD>(std::size(hostBuf));
    uc.lpszUrlPath = pathBuf;
    uc.dwUrlPathLength = static_cast<DWORD>(std::size(pathBuf));
    uc.lpszExtraInfo = extraBuf;
    uc.dwExtraInfoLength = static_cast<DWORD>(std::size(extraBuf));

    if (!::WinHttpCrackUrl(url.c_str(), static_cast<DWORD>(url.size()), 0, &uc)) {
        return false;
    }
    if (uc.nScheme != INTERNET_SCHEME_HTTPS || uc.dwHostNameLength == 0) {
        return false;
    }
    out.host.assign(uc.lpszHostName, uc.dwHostNameLength);
    out.port = uc.nPort;
    out.pathAndQuery.assign(uc.lpszUrlPath, uc.dwUrlPathLength);
    if (uc.dwExtraInfoLength > 0) {
        out.pathAndQuery.append(uc.lpszExtraInfo, uc.dwExtraInfoLength);
    }
    if (out.pathAndQuery.empty()) {
        out.pathAndQuery = L"/";
    }
    return true;
}

HINTERNET openUpdaterSession() {
    HINTERNET h = ::WinHttpOpen(
        L"ApkaGitarowaUpdater/1.0",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (h == nullptr) {
        h = ::WinHttpOpen(
            L"ApkaGitarowaUpdater/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);
    }
    if (h != nullptr) {
        DWORD protos = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2 | 0x00002000u;
        if (!::WinHttpSetOption(h, WINHTTP_OPTION_SECURE_PROTOCOLS, &protos, sizeof(protos))) {
            protos = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
            ::WinHttpSetOption(h, WINHTTP_OPTION_SECURE_PROTOCOLS, &protos, sizeof(protos));
        }
    }
    return h;
}

struct SplashState {
    enum class Phase : int {
        Checking = 0,
        Downloading = 1,
        Applying = 2,
        Done = 3
    };

    std::atomic<int> phase{static_cast<int>(Phase::Checking)};
    std::atomic<bool> cancelRequested{false};
    std::atomic<bool> workerDone{false};
    std::atomic<std::uint64_t> bytesReceived{0};
    std::atomic<std::uint64_t> bytesTotal{0};

    std::mutex textMutex;
    std::wstring statusText = L"Sprawdzanie aktualizacji...";

    HWND hwnd = nullptr;
    HFONT titleFont = nullptr;
    HFONT bodyFont = nullptr;
    HICON appIcon = nullptr;
    bool skipHovered = false;
    int animTick = 0;

    void setStatus(const std::wstring& msg) {
        std::lock_guard<std::mutex> lock(textMutex);
        statusText = msg;
    }

    std::wstring getStatus() {
        std::lock_guard<std::mutex> lock(textMutex);
        return statusText;
    }
};

void drawRoundRectFilled(HDC hdc, const RECT& rc, int radius, COLORREF fillCol, COLORREF borderCol) {
    HBRUSH br = ::CreateSolidBrush(fillCol);
    HPEN pen = ::CreatePen(PS_SOLID, 1, borderCol);
    HGDIOBJ oldBr = ::SelectObject(hdc, br);
    HGDIOBJ oldPen = ::SelectObject(hdc, pen);
    ::RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    ::SelectObject(hdc, oldBr);
    ::SelectObject(hdc, oldPen);
    ::DeleteObject(br);
    ::DeleteObject(pen);
}

void paintSplashWindow(HWND hwnd, SplashState* st) {
    PAINTSTRUCT ps{};
    HDC hdc = ::BeginPaint(hwnd, &ps);

    RECT client{};
    ::GetClientRect(hwnd, &client);

    HDC memDc = ::CreateCompatibleDC(hdc);
    HBITMAP memBmp = ::CreateCompatibleBitmap(hdc, client.right, client.bottom);
    HGDIOBJ oldBmp = ::SelectObject(memDc, memBmp);

    // Background & 1px border (#14110F & #332B24)
    HBRUSH bgBrush = ::CreateSolidBrush(RGB(0x14, 0x11, 0x0F));
    ::FillRect(memDc, &client, bgBrush);
    ::DeleteObject(bgBrush);

    HBRUSH borderBrush = ::CreateSolidBrush(RGB(0x33, 0x2B, 0x24));
    ::FrameRect(memDc, &client, borderBrush);
    ::DeleteObject(borderBrush);

    ::SetBkMode(memDc, TRANSPARENT);

    // Icon or amber badge
    int titleX = 20;
    if (st->appIcon != nullptr) {
        ::DrawIconEx(memDc, 20, 18, st->appIcon, 24, 24, 0, nullptr, DI_NORMAL);
        titleX = 52;
    } else {
        RECT dotRc{20, 22, 34, 36};
        drawRoundRectFilled(memDc, dotRc, 6, RGB(0xF3, 0x92, 0x37), RGB(0xF3, 0x92, 0x37));
        titleX = 44;
    }

    // Title
    HGDIOBJ oldFont = ::SelectObject(memDc, st->titleFont ? st->titleFont : ::GetStockObject(DEFAULT_GUI_FONT));
    ::SetTextColor(memDc, RGB(0xF5, 0xF2, 0xEB));
    RECT titleRc{titleX, 18, kSplashWidth - 20, 44};
    ::DrawTextW(memDc, L"Apka Gitarowa", -1, &titleRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Status text
    ::SelectObject(memDc, st->bodyFont ? st->bodyFont : ::GetStockObject(DEFAULT_GUI_FONT));
    ::SetTextColor(memDc, RGB(0x9C, 0xA3, 0xAF));
    const std::wstring status = st->getStatus();
    RECT statusRc{20, 52, kSplashWidth - 20, 74};
    ::DrawTextW(memDc, status.c_str(), -1, &statusRc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // Progress bar track
    RECT trackRc{20, 84, kSplashWidth - 20, 96};
    drawRoundRectFilled(memDc, trackRc, 6, RGB(0x21, 0x1C, 0x18), RGB(0x33, 0x2B, 0x24));

    const int trackW = (trackRc.right - trackRc.left) - 2;
    const auto phase = static_cast<SplashState::Phase>(st->phase.load());
    const std::uint64_t recv = st->bytesReceived.load();
    const std::uint64_t total = st->bytesTotal.load();

    if (phase == SplashState::Phase::Downloading && total > 0) {
        const double frac = std::min(1.0, static_cast<double>(recv) / static_cast<double>(total));
        const int fillW = std::max(8, static_cast<int>(frac * trackW));
        RECT fillRc{trackRc.left + 1, trackRc.top + 1, trackRc.left + 1 + fillW, trackRc.bottom - 1};
        drawRoundRectFilled(memDc, fillRc, 5, RGB(0xF3, 0x92, 0x37), RGB(0xF3, 0x92, 0x37));
    } else if (phase == SplashState::Phase::Applying) {
        RECT fillRc{trackRc.left + 1, trackRc.top + 1, trackRc.right - 1, trackRc.bottom - 1};
        drawRoundRectFilled(memDc, fillRc, 5, RGB(0x4A, 0xDE, 0x80), RGB(0x4A, 0xDE, 0x80));
    } else {
        const int segW = 96;
        const int travel = trackW + segW;
        const int pos = (st->animTick * 10) % travel;
        const int segL = std::max(trackRc.left + 1, trackRc.left + 1 + pos - segW);
        const int segR = std::min(trackRc.right - 1, trackRc.left + 1 + pos);
        if (segR > segL) {
            RECT fillRc{segL, trackRc.top + 1, segR, trackRc.bottom - 1};
            drawRoundRectFilled(memDc, fillRc, 5, RGB(0xF3, 0x92, 0x37), RGB(0xF3, 0x92, 0x37));
        }
    }

    // "Pomiń" button
    const COLORREF btnFill = st->skipHovered ? RGB(0x2C, 0x25, 0x20) : RGB(0x21, 0x1C, 0x18);
    drawRoundRectFilled(memDc, kSkipBtnRect, 8, btnFill, RGB(0x33, 0x2B, 0x24));
    ::SetTextColor(memDc, st->cancelRequested.load() ? RGB(0x9C, 0xA3, 0xAF) : RGB(0xF5, 0xF2, 0xEB));
    RECT btnTextRc = kSkipBtnRect;
    ::DrawTextW(memDc, L"Pomi\u0144", -1, &btnTextRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    ::SelectObject(memDc, oldFont);

    ::BitBlt(hdc, 0, 0, client.right, client.bottom, memDc, 0, 0, SRCCOPY);
    ::SelectObject(memDc, oldBmp);
    ::DeleteObject(memBmp);
    ::DeleteDC(memDc);
    ::EndPaint(hwnd, &ps);
}

LRESULT CALLBACK splashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* st = reinterpret_cast<SplashState*>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_NCCREATE: {
            const auto* cs = reinterpret_cast<const CREATESTRUCTW*>(lParam);
            ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            if (st != nullptr) {
                paintSplashWindow(hwnd, st);
                return 0;
            }
            break;
        case WM_TIMER:
            if (st == nullptr) break;
            if (wParam == kTimerShowSplash) {
                ::KillTimer(hwnd, kTimerShowSplash);
                if (!st->workerDone.load() && !st->cancelRequested.load()) {
                    ::ShowWindow(hwnd, SW_SHOW);
                    ::UpdateWindow(hwnd);
                }
            } else if (wParam == kTimerAnimate) {
                ++st->animTick;
                if (::IsWindowVisible(hwnd)) {
                    ::InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        case WM_GITARA_PHASE_CHANGED:
            if (st != nullptr && !st->workerDone.load() && !st->cancelRequested.load()) {
                ::KillTimer(hwnd, kTimerShowSplash);
                if (!::IsWindowVisible(hwnd)) {
                    ::ShowWindow(hwnd, SW_SHOW);
                }
                ::InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_GITARA_WORKER_DONE:
            ::DestroyWindow(hwnd);
            return 0;
        case WM_MOUSEMOVE:
            if (st != nullptr) {
                POINT pt{static_cast<LONG>(static_cast<short>(LOWORD(lParam))),
                         static_cast<LONG>(static_cast<short>(HIWORD(lParam)))};
                const bool inside = ::PtInRect(&kSkipBtnRect, pt) != FALSE;
                if (inside != st->skipHovered) {
                    st->skipHovered = inside;
                    ::InvalidateRect(hwnd, &kSkipBtnRect, FALSE);
                }
            }
            return 0;
        case WM_LBUTTONUP:
            if (st != nullptr) {
                POINT pt{static_cast<LONG>(static_cast<short>(LOWORD(lParam))),
                         static_cast<LONG>(static_cast<short>(HIWORD(lParam)))};
                if (::PtInRect(&kSkipBtnRect, pt)) {
                    st->cancelRequested.store(true);
                    st->setStatus(L"Pomijanie aktualizacji...");
                    ::InvalidateRect(hwnd, nullptr, FALSE);
                }
            }
            return 0;
        case WM_KEYDOWN:
            if (st != nullptr && wParam == VK_ESCAPE) {
                st->cancelRequested.store(true);
                st->setStatus(L"Pomijanie aktualizacji...");
                ::InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;
        case WM_CLOSE:
            if (st != nullptr) {
                st->cancelRequested.store(true);
                st->setStatus(L"Pomijanie aktualizacji...");
                ::InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_DESTROY:
            ::KillTimer(hwnd, kTimerShowSplash);
            ::KillTimer(hwnd, kTimerAnimate);
            ::PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return ::DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool fetchTextWinHttp(
    HINTERNET hSession,
    const std::wstring& url,
    std::string& outBody,
    std::size_t maxBytes,
    const std::atomic<bool>& cancelFlag) {
    outBody.clear();
    ParsedHttpsUrl parsed{};
    if (!crackHttpsUrl(url, parsed)) return false;

    WinHttpHandle hConnect(::WinHttpConnect(hSession, parsed.host.c_str(), parsed.port, 0));
    if (!hConnect.valid()) return false;

    WinHttpHandle hRequest(::WinHttpOpenRequest(
        hConnect.handle,
        L"GET",
        parsed.pathAndQuery.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE | WINHTTP_FLAG_REFRESH));
    if (!hRequest.valid()) return false;

    ::WinHttpSetTimeouts(hRequest.handle, 2000, 2500, 2500, 3500);
    DWORD redirPolicy = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    ::WinHttpSetOption(hRequest.handle, WINHTTP_OPTION_REDIRECT_POLICY, &redirPolicy, sizeof(redirPolicy));

    const wchar_t* headers = L"Cache-Control: no-cache\r\nPragma: no-cache\r\n";
    if (!::WinHttpSendRequest(
            hRequest.handle,
            headers,
            static_cast<DWORD>(-1L),
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0)) {
        return false;
    }
    if (cancelFlag.load()) return false;

    if (!::WinHttpReceiveResponse(hRequest.handle, nullptr)) {
        return false;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (!::WinHttpQueryHeaders(
            hRequest.handle,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &statusCode,
            &statusSize,
            WINHTTP_NO_HEADER_INDEX) ||
        statusCode != 200) {
        return false;
    }

    char buf[8192];
    for (;;) {
        if (cancelFlag.load()) return false;
        DWORD avail = 0;
        if (!::WinHttpQueryDataAvailable(hRequest.handle, &avail)) return false;
        if (avail == 0) break;
        const DWORD toRead = std::min<DWORD>(avail, static_cast<DWORD>(sizeof(buf)));
        DWORD readBytes = 0;
        if (!::WinHttpReadData(hRequest.handle, buf, toRead, &readBytes)) return false;
        if (readBytes == 0) break;
        if (outBody.size() + readBytes > maxBytes) return false;
        outBody.append(buf, readBytes);
    }
    return !outBody.empty();
}

bool downloadFileWinHttp(
    HINTERNET hSession,
    const std::wstring& url,
    const std::filesystem::path& destPath,
    std::uint64_t expectedSizeHint,
    SplashState& st,
    std::string& outSha256Hex,
    std::uint64_t& outDownloadedSize) {
    outSha256Hex.clear();
    outDownloadedSize = 0;

    ParsedHttpsUrl parsed{};
    if (!crackHttpsUrl(url, parsed)) return false;

    // Pre-verify write permission before starting download
    HANDLE hFile = ::CreateFileW(
        destPath.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    auto cleanupFile = [&]() {
        if (hFile != INVALID_HANDLE_VALUE) {
            ::CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }
        ::DeleteFileW(destPath.c_str());
    };

    WinHttpHandle hConnect(::WinHttpConnect(hSession, parsed.host.c_str(), parsed.port, 0));
    if (!hConnect.valid()) {
        cleanupFile();
        return false;
    }

    WinHttpHandle hRequest(::WinHttpOpenRequest(
        hConnect.handle,
        L"GET",
        parsed.pathAndQuery.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE | WINHTTP_FLAG_REFRESH));
    if (!hRequest.valid()) {
        cleanupFile();
        return false;
    }

    ::WinHttpSetTimeouts(hRequest.handle, 2500, 4000, 4000, 30000);
    DWORD redirPolicy = WINHTTP_OPTION_REDIRECT_POLICY_DISALLOW_HTTPS_TO_HTTP;
    ::WinHttpSetOption(hRequest.handle, WINHTTP_OPTION_REDIRECT_POLICY, &redirPolicy, sizeof(redirPolicy));

    if (!::WinHttpSendRequest(
            hRequest.handle,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            WINHTTP_NO_REQUEST_DATA,
            0,
            0,
            0) ||
        !::WinHttpReceiveResponse(hRequest.handle, nullptr)) {
        cleanupFile();
        return false;
    }

    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (!::WinHttpQueryHeaders(
            hRequest.handle,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &statusCode,
            &statusSize,
            WINHTTP_NO_HEADER_INDEX) ||
        statusCode != 200) {
        cleanupFile();
        return false;
    }

    std::uint64_t totalSize = expectedSizeHint;
    wchar_t lenBuf[64]{};
    DWORD lenBufBytes = sizeof(lenBuf);
    if (::WinHttpQueryHeaders(
            hRequest.handle,
            WINHTTP_QUERY_CONTENT_LENGTH,
            WINHTTP_HEADER_NAME_BY_INDEX,
            lenBuf,
            &lenBufBytes,
            WINHTTP_NO_HEADER_INDEX)) {
        wchar_t* endPtr = nullptr;
        const unsigned long long parsedLen = std::wcstoull(lenBuf, &endPtr, 10);
        if (endPtr != lenBuf && parsedLen > 0) {
            totalSize = static_cast<std::uint64_t>(parsedLen);
        }
    }
    st.bytesTotal.store(totalSize);

    constexpr std::uint64_t kMaxExeBytes = 64ull * 1024ull * 1024ull;
    if (totalSize > kMaxExeBytes) {
        cleanupFile();
        return false;
    }

    Sha256 hasher;
    std::vector<std::uint8_t> chunk(65536);
    std::uint8_t headerMagic[2]{};
    std::size_t magicCount = 0;

    for (;;) {
        if (st.cancelRequested.load()) {
            cleanupFile();
            return false;
        }
        DWORD avail = 0;
        if (!::WinHttpQueryDataAvailable(hRequest.handle, &avail)) {
            cleanupFile();
            return false;
        }
        if (avail == 0) break;

        const DWORD toRead = std::min<DWORD>(avail, static_cast<DWORD>(chunk.size()));
        DWORD readBytes = 0;
        if (!::WinHttpReadData(hRequest.handle, chunk.data(), toRead, &readBytes)) {
            cleanupFile();
            return false;
        }
        if (readBytes == 0) break;
        if (outDownloadedSize + readBytes > kMaxExeBytes) {
            cleanupFile();
            return false;
        }

        for (DWORD i = 0; i < readBytes && magicCount < 2; ++i) {
            headerMagic[magicCount++] = chunk[i];
        }

        DWORD written = 0;
        if (!::WriteFile(hFile, chunk.data(), readBytes, &written, nullptr) || written != readBytes) {
            cleanupFile();
            return false;
        }

        hasher.update(chunk.data(), readBytes);
        outDownloadedSize += readBytes;
        st.bytesReceived.store(outDownloadedSize);

        if (totalSize > 0) {
            const int pct = static_cast<int>(std::min<std::uint64_t>(100, (outDownloadedSize * 100ull) / totalSize));
            const double gotMb = static_cast<double>(outDownloadedSize) / (1024.0 * 1024.0);
            const double totMb = static_cast<double>(totalSize) / (1024.0 * 1024.0);
            wchar_t msg[128]{};
            std::swprintf(
                msg, std::size(msg),
                L"Pobieranie aktualizacji: %d%% (%.1f / %.1f MB)",
                pct, gotMb, totMb);
            st.setStatus(msg);
        } else {
            const double gotMb = static_cast<double>(outDownloadedSize) / (1024.0 * 1024.0);
            wchar_t msg[128]{};
            std::swprintf(msg, std::size(msg), L"Pobieranie aktualizacji: %.1f MB", gotMb);
            st.setStatus(msg);
        }
    }

    ::FlushFileBuffers(hFile);
    ::CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    if (outDownloadedSize <= 4096 || magicCount < 2 || headerMagic[0] != 'M' || headerMagic[1] != 'Z') {
        ::DeleteFileW(destPath.c_str());
        return false;
    }

    outSha256Hex = hasher.finalHex();
    return true;
}

} // namespace

void cleanupPreviousUpdateArtifacts() {
    const std::filesystem::path exePath = getExecutablePath();
    if (exePath.empty()) return;

    std::filesystem::path newPath = exePath;
    newPath += L".new";
    ::DeleteFileW(newPath.c_str());

    std::filesystem::path oldPath = exePath;
    oldPath += L".old";
    std::error_code ec;
    if (!std::filesystem::exists(oldPath, ec)) return;

    for (int i = 0; i < 12; ++i) {
        if (::DeleteFileW(oldPath.c_str())) {
            break;
        }
        const DWORD err = ::GetLastError();
        if (err == ERROR_FILE_NOT_FOUND) break;
        ::Sleep(100);
    }
}

AutoUpdateOutcome runPreLaunchAutoUpdate(const AutoUpdateOptions& opts) {
    AutoUpdateOutcome outcome{};
    if (!opts.forceCheck && (opts.currentCommit.empty() || opts.currentCommit == "dev")) {
        return outcome;
    }
    if (opts.repo.empty() || opts.tag.empty()) {
        return outcome;
    }

    const std::filesystem::path exePath = getExecutablePath();
    if (exePath.empty()) {
        return outcome;
    }

    SplashState state{};
    HINSTANCE hInst = ::GetModuleHandleW(nullptr);
    state.appIcon = static_cast<HICON>(
        ::LoadImageW(hInst, MAKEINTRESOURCEW(102), IMAGE_ICON, 24, 24, LR_SHARED));
    state.titleFont = ::CreateFontW(
        -16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    state.bodyFont = ::CreateFontW(
        -13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = splashWndProc;
    wc.hInstance = hInst;
    wc.hCursor = ::LoadCursorW(nullptr, MAKEINTRESOURCEW(32512)); // IDC_ARROW (wariant W bez UNICODE)
    wc.lpszClassName = L"ApkaGitarowaUpdaterSplash";
    ::RegisterClassExW(&wc);

    POINT origin{0, 0};
    HMONITOR mon = ::MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    int posX = 120;
    int posY = 120;
    if (::GetMonitorInfoW(mon, &mi)) {
        posX = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - kSplashWidth) / 2;
        posY = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - kSplashHeight) / 2;
    }

    HWND hwnd = ::CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"Apka Gitarowa",
        WS_POPUP,
        posX, posY, kSplashWidth, kSplashHeight,
        nullptr, nullptr, hInst, &state);
    state.hwnd = hwnd;

    if (hwnd != nullptr) {
        ::SetTimer(hwnd, kTimerShowSplash, 250, nullptr);
        ::SetTimer(hwnd, kTimerAnimate, 40, nullptr);
    }

    std::thread worker([&]() {
        auto notifyDone = [&]() {
            state.workerDone.store(true);
            if (state.hwnd != nullptr) {
                ::PostMessageW(state.hwnd, WM_GITARA_WORKER_DONE, 0, 0);
            }
        };

        WinHttpHandle hSession(openUpdaterSession());
        if (!hSession.valid()) {
            notifyDone();
            return;
        }

        const std::wstring manifestUrl = utf8ToWide(buildManifestUrl(opts.repo, opts.tag));
        std::string manifestJson;
        if (!fetchTextWinHttp(hSession.handle, manifestUrl, manifestJson, 64u * 1024u, state.cancelRequested)) {
            notifyDone();
            return;
        }

        const auto manifest = parseUpdateManifest(manifestJson);
        if (!manifest.has_value()) {
            notifyDone();
            return;
        }

        const auto decision = evaluateUpdate(opts.currentCommit, *manifest, opts.repo, opts.forceCheck);
        if (decision != UpdateCheckDecision::UpdateAvailable) {
            notifyDone();
            return;
        }

        state.phase.store(static_cast<int>(SplashState::Phase::Downloading));
        state.bytesTotal.store(manifest->size);
        state.setStatus(L"Pobieranie aktualizacji...");
        if (state.hwnd != nullptr) {
            ::PostMessageW(state.hwnd, WM_GITARA_PHASE_CHANGED, 0, 0);
        }

        std::filesystem::path newPath = exePath;
        newPath += L".new";
        std::filesystem::path oldPath = exePath;
        oldPath += L".old";

        std::string actualSha;
        std::uint64_t downloadedSize = 0;
        const std::wstring dlUrl = utf8ToWide(manifest->downloadUrl);
        if (!downloadFileWinHttp(
                hSession.handle, dlUrl, newPath, manifest->size,
                state, actualSha, downloadedSize)) {
            notifyDone();
            return;
        }

        if (state.cancelRequested.load() ||
            (manifest->size > 0 && downloadedSize != manifest->size) ||
            !iequalsAscii(actualSha, manifest->sha256)) {
            ::DeleteFileW(newPath.c_str());
            notifyDone();
            return;
        }

        state.phase.store(static_cast<int>(SplashState::Phase::Applying));
        state.setStatus(L"Instalowanie aktualizacji...");
        if (state.hwnd != nullptr) {
            ::PostMessageW(state.hwnd, WM_GITARA_PHASE_CHANGED, 0, 0);
        }

        ::DeleteFileW(oldPath.c_str());
        if (!::MoveFileExW(exePath.c_str(), oldPath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            ::DeleteFileW(newPath.c_str());
            notifyDone();
            return;
        }
        if (!::MoveFileExW(newPath.c_str(), exePath.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            ::MoveFileExW(oldPath.c_str(), exePath.c_str(), MOVEFILE_REPLACE_EXISTING);
            ::DeleteFileW(newPath.c_str());
            notifyDone();
            return;
        }

        outcome.result = AutoUpdateResult::UpdatedAndRelaunchReady;
        outcome.relaunchExePath = exePath;
        state.phase.store(static_cast<int>(SplashState::Phase::Done));
        notifyDone();
    });

    if (hwnd != nullptr) {
        MSG msg{};
        while (::GetMessageW(&msg, nullptr, 0, 0) > 0) {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
    }

    if (worker.joinable()) {
        worker.join();
    }

    if (state.titleFont != nullptr) ::DeleteObject(state.titleFont);
    if (state.bodyFont != nullptr) ::DeleteObject(state.bodyFont);
    ::UnregisterClassW(wc.lpszClassName, hInst);
    return outcome;
}

bool spawnRelaunchedApp(
    const std::filesystem::path& exePath,
    const std::vector<std::string>& forwardArgs) {
    if (exePath.empty()) return false;

    std::wstring cmdLine = quoteWinArg(exePath.wstring());
    cmdLine += L" --skip-update-once";
    for (const auto& arg : forwardArgs) {
        if (arg == "--skip-update-once" || arg == "--force-update-check") continue;
        cmdLine.push_back(L' ');
        cmdLine += quoteWinArg(utf8ToWide(arg));
    }

    std::vector<wchar_t> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(L'\0');

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};
    if (!::CreateProcessW(
            exePath.c_str(),
            cmdBuf.data(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi)) {
        return false;
    }

    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);
    return true;
}

} // namespace gitara::platform

#else

namespace gitara::platform {

void cleanupPreviousUpdateArtifacts() {}

AutoUpdateOutcome runPreLaunchAutoUpdate(const AutoUpdateOptions& opts) {
    (void)opts;
    return AutoUpdateOutcome{};
}

bool spawnRelaunchedApp(
    const std::filesystem::path& exePath,
    const std::vector<std::string>& forwardArgs) {
    (void)exePath;
    (void)forwardArgs;
    return false;
}

} // namespace gitara::platform

#endif
