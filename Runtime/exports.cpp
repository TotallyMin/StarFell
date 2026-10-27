#include "Extension/Assets/native_patch_support.h"
#include "Engine/Game/Build/supported_build.h"
#include "Engine/Game/Build/20260929/route_lookahead.h"
#include <array>

// Static PE-readable capability data. Studio checks these without loading the
// DLL; this advertises implementation support, not a claim about a live hook.
extern "C" {
__declspec(dllexport) extern constinit const starfell::game::build::v20260929::route_lookahead::RouteLookaheadContract
    StarFellNpcRouteCycleGuardV1 = starfell::game::build::v20260929::route_lookahead::route_lookahead_contract;
__declspec(dllexport) extern constinit const starfell::game::build::v20260929::route_lookahead::RouteLookaheadContract
    StarFellNpcRouteEndpointGuardV1 = starfell::game::build::v20260929::route_lookahead::route_endpoint_contract;
__declspec(dllexport) extern constinit const starfell::game::build::v20260929::route_lookahead::RouteLookaheadContract
    StarFellNpcRouteReverseEndpointGuardV1 = starfell::game::build::v20260929::route_lookahead::route_reverse_endpoint_contract;
__declspec(dllexport) extern constinit const std::array<starfell::game::build::v20260929::route_lookahead::RouteLookaheadContract, 2>
    StarFellNpcRouteProjectionGuardV1{starfell::game::build::v20260929::route_lookahead::route_array_projection_contract,
                                   starfell::game::build::v20260929::route_lookahead::route_vector_projection_contract};
__declspec(dllexport) extern constinit const std::uint32_t StarFellNativePatchSupportVersion =
    starfell::native_patch_support_version;
__declspec(dllexport) extern constinit const std::array<char, 65> StarFellNativePatchSupportGameSha256 = [] {
    std::array<char, 65> value{};
    for (std::size_t i = 0; i < 64; ++i) value[i] = starfell::supported_build::game_sha256[i];
    return value;
}();
}

