#pragma once
#include <chrono>
#include <cstdint>

namespace starfell::multiplayer {
// Microseconds on the steady clock. Session, audio and animation timings share it.
inline std::uint64_t now_us() noexcept {
    return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
                                          std::chrono::steady_clock::now().time_since_epoch())
                                          .count());
}
} // namespace starfell::multiplayer
