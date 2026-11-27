#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// Bez GDI: windows.h deklaruje funkcje Chord()/Arc()/Pie() kolidujace z gitara::theory::Chord.
#ifndef NOGDI
#define NOGDI
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <csignal>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cstdint>
#include <string>

namespace gitara::platform {

#ifdef _WIN32
using SocketHandle = SOCKET;
inline constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
using SocketHandle = int;
inline constexpr SocketHandle kInvalidSocket = -1;
#endif

class SocketInit {
public:
    SocketInit() noexcept {
#ifdef _WIN32
        WSADATA wsa{};
        ok_ = (::WSAStartup(MAKEWORD(2, 2), &wsa) == 0);
#else
        std::signal(SIGPIPE, SIG_IGN);
        ok_ = true;
#endif
    }

    ~SocketInit() {
#ifdef _WIN32
        if (ok_) {
            ::WSACleanup();
        }
#endif
    }

    SocketInit(const SocketInit&) = delete;
    SocketInit& operator=(const SocketInit&) = delete;

    [[nodiscard]] bool ok() const noexcept { return ok_; }

private:
    bool ok_ = false;
};

inline void closeSocket(SocketHandle sock) noexcept {
    if (sock == kInvalidSocket) return;
#ifdef _WIN32
    ::closesocket(sock);
#else
    ::close(sock);
#endif
}

inline void setReuseAddr(SocketHandle sock) noexcept {
    int opt = 1;
#ifdef _WIN32
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
#else
    ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
}

inline void setRecvTimeoutMs(SocketHandle sock, int timeoutMs) noexcept {
#ifdef _WIN32
    const DWORD tv = static_cast<DWORD>(timeoutMs > 0 ? timeoutMs : 0);
    ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
#else
    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
}

inline void setSendTimeoutMs(SocketHandle sock, int timeoutMs) noexcept {
#ifdef _WIN32
    const DWORD tv = static_cast<DWORD>(timeoutMs > 0 ? timeoutMs : 0);
    ::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof(tv));
#else
    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    ::setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
#endif
}

/// Returns >0 if readable, 0 on timeout, <0 on error.
inline int waitReadableMs(SocketHandle sock, int timeoutMs) noexcept {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(sock, &readSet);

    timeval tv{};
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

#ifdef _WIN32
    return ::select(0, &readSet, nullptr, nullptr, &tv);
#else
    return ::select(sock + 1, &readSet, nullptr, nullptr, &tv);
#endif
}

inline int socketRecv(SocketHandle sock, char* buf, int len) noexcept {
#ifdef _WIN32
    return ::recv(sock, buf, len, 0);
#else
    return static_cast<int>(::recv(sock, buf, static_cast<std::size_t>(len), 0));
#endif
}

inline int socketSend(SocketHandle sock, const char* buf, int len) noexcept {
#ifdef _WIN32
    return ::send(sock, buf, len, 0);
#else
    return static_cast<int>(::send(sock, buf, static_cast<std::size_t>(len), 0));
#endif
}

inline bool probeLocalhostPing(int port, int timeoutMs = 400) noexcept {
    if (port <= 0 || port > 65535) return false;
    SocketHandle sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == kInvalidSocket) return false;

    setRecvTimeoutMs(sock, timeoutMs);
    setSendTimeoutMs(sock, timeoutMs);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(static_cast<std::uint16_t>(port));

    if (::connect(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0) {
        closeSocket(sock);
        return false;
    }

    static constexpr const char kReq[] =
        "GET /api/ping HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: close\r\n\r\n";
    const int reqLen = static_cast<int>(sizeof(kReq) - 1);
    if (socketSend(sock, kReq, reqLen) != reqLen) {
        closeSocket(sock);
        return false;
    }

    std::string resp;
    char buf[512];
    for (;;) {
        const int n = socketRecv(sock, buf, sizeof(buf));
        if (n <= 0) break;
        resp.append(buf, static_cast<std::size_t>(n));
        if (resp.size() >= 4096) break;
    }
    closeSocket(sock);

    return resp.find("200 OK") != std::string::npos &&
           resp.find("\"ok\":true") != std::string::npos;
}

} // namespace gitara::platform
