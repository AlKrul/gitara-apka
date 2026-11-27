#pragma once

#include "app/platform/AppPaths.h"

#include <cstdint>

namespace gitara::platform {

class SingleInstanceLock {
public:
    SingleInstanceLock() = default;
    ~SingleInstanceLock();

    SingleInstanceLock(const SingleInstanceLock&) = delete;
    SingleInstanceLock& operator=(const SingleInstanceLock&) = delete;

    [[nodiscard]] bool acquire(int retryMs = 0);
    void release() noexcept;

private:
#ifdef _WIN32
    void* mutex_ = nullptr;
#endif
};

struct BrowserLaunchResult {
    bool launched = false;
    bool dedicatedProcess = false;
    std::uintptr_t nativeHandle = 0;
};

[[nodiscard]] BrowserLaunchResult launchAppWindow(int port, const AppPaths& paths);
[[nodiscard]] bool isBrowserProcessExited(const BrowserLaunchResult& res) noexcept;
void closeBrowserHandle(BrowserLaunchResult& res) noexcept;
void attachConsoleIfPresent() noexcept;

} // namespace gitara::platform
