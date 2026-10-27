#pragma once
#include <cstdint>
#include <string>
namespace starfell {
bool start_custom_script_loader(std::uintptr_t base, std::string& error);
}
