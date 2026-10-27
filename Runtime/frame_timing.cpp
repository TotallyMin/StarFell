#include "runtime_internal.h"
#include "Engine/Core/Hooks/hooks.h"
#include "Engine/Core/Profiling/profiler.h"
#include "Engine/Game/Build/addresses.h"
#include "Engine/Game/Build/20260929/profiler_labels.h"

// The game's client frame job, timed for the profiler (Engine/Core/Profiling/profiler.h). It runs
// the whole client frame, StarFell's tick included, so its time against the sim budget (16.7 ms at
// 60 Hz) says how close the game is to slow motion. Two clock reads per client frame.
namespace starfell::runtime::detail {
namespace {
using ClientFrameJob = std::uintptr_t (*)(std::uintptr_t, std::uintptr_t, std::uintptr_t, std::uintptr_t);
ClientFrameJob original_client_frame_job{};
std::uintptr_t client_frame_job(std::uintptr_t job, std::uintptr_t timing, std::uintptr_t dependencies, std::uintptr_t extra) {
    const auto start = starfell::profiler::now_ns();
    const auto result = original_client_frame_job(job, timing, dependencies, extra);
    starfell::profiler::record_client_frame(starfell::profiler::now_ns() - start);
    return result;
}
}
void start_frame_timing() {
    const auto& contract = addr::profiler_labels::client_frame_job;
    auto* target = reinterpret_cast<void*>(runtime().base + contract.rva);
    bool enabled = false;
    if (matches(reinterpret_cast<std::uintptr_t>(target), contract.bytes)) {
        void* original{};
        if (starfell::hook_prepare(target, reinterpret_cast<void*>(&client_frame_job), &original) == starfell::HookOk) {
            original_client_frame_job = reinterpret_cast<ClientFrameJob>(original);
            enabled = starfell::hook_enable(target) == starfell::HookOk;
            if (!enabled) (void)starfell::hook_remove(target);
        }
    }
    record(std::string("{\"event\":\"frame_timing_initialized\",\"active\":") + (enabled ? "true}" : "false}"));
}
}
