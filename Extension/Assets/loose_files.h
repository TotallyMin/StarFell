#pragma once
#include <cstdint>
#include <string>

namespace starfell {
// Install before native startup so startup configs and Lua use the same overlay.
bool start_loose_files(std::uintptr_t base, std::string& error);
}
