#pragma once

#include "ebx_document.h"

#include <cstdint>
#include <vector>

namespace starfell::frostbite::ebx {

[[nodiscard]] std::uint16_t ensure_fixup_type(Document& document, std::int32_t descriptor);

[[nodiscard]] std::vector<std::byte> write_document(const Document& document);

}
