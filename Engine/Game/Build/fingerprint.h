#pragma once
#include <array>
#include <cstdint>

namespace starfell::game::build {
struct Fingerprint {
    std::uintptr_t rva;
    std::array<unsigned char, 32> bytes;
};
}
