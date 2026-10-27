#include "runtime_internal.h"
#include "Engine/Core/Log/logging.h"
#include "Extension/Progression/fixed_stop_entitlement_provider.h"
#include "Extension/Progression/mission_progression_override.h"
#include "Extension/Progression/neighborhood_unlock_override.h"
#include "Extension/Settings/named_settings.h"
#include "Extension/Skater/skater_slot_override.h"
#include <atomic>
#include <memory>

namespace starfell::runtime::detail {
Runtime& runtime() { static auto* value = new Runtime; return *value; }

// The destinations mods and the Patch folder declare: every mod's
// starfell-levels.json (highest priority first), then the Patch folders'.
starfell::CustomLevelManifest read_custom_levels(const std::vector<std::filesystem::path>& mod_manifests) {
    auto paths = mod_manifests;
    const auto& patch = runtime().patch_level_manifests;
    paths.insert(paths.end(), patch.begin(), patch.end());
    std::vector<starfell::CustomLevelManifest> manifests;
    for (const auto& path : paths) {
        auto manifest = starfell::read_custom_level_manifest(path);
        if (!manifest.present) continue;
        if (manifest.issue.empty()) {
            starfell::logging::log(starfell::logging::Level::info, starfell::logging::Channel::level,
                "Custom-level manifest accepted: {} destination(s) from {}.", manifest.levels.size(), path.string());
        } else {
            starfell::logging::log(starfell::logging::Level::warning, starfell::logging::Channel::level,
                "Custom-level manifest ignored: {} ({}).", manifest.issue, path.string());
        }
        manifests.push_back(std::move(manifest));
    }
    return starfell::combine_custom_level_manifests(manifests);
}
starfell::runtime::RequestContext request_context(const Runtime& value) {
    return {value.native_tick_ready, !value.observer_failed, value.model.can_queue_load,
        value.offline_model.available};
}

void publish_named_settings(Runtime& value) {
    const auto revision = starfell::named_settings_revision();
    if (revision == value.named_settings_revision) return;
    // Refill the previous snapshot once no reader holds it: its rows keep their
    // string buffers, where a new one allocates thousands of strings.
    auto next = std::move(value.named_settings_spare);
    if (next && next.use_count() == 1) std::atomic_thread_fence(std::memory_order_acquire);
    else next = std::make_shared<NamedSettings>();
    *next = starfell::named_settings_model();
    std::shared_ptr<const NamedSettings> previous = std::move(next);
    {
        std::lock_guard lock(value.mutex);
        value.named_settings.swap(previous);
        value.named_settings_revision = revision;
    }
    value.named_settings_spare = std::const_pointer_cast<NamedSettings>(std::move(previous));
}

void merge_skater_slot_observation(
    starfell::overlay::OfflineFeatureModel& model,
    const starfell::SkaterSlotOverrideObservation& slots) {
    model.skater_slots_available = slots.available;
    model.skater_slots_override_active = slots.requested;
    model.skater_slot_manager_available = slots.manager_available;
    model.skater_slot_selectors_enabled = slots.selectors_enabled;
    model.skater_slot_count = slots.slot_count;
    model.skater_slot_target = slots.target_slot_count;
    model.skater_slot_status = slots.status;
    model.settings_owned = model.settings_owned || slots.settings_owned;
}

void merge_main_mission_observation(
    starfell::overlay::OfflineFeatureModel& model,
    const starfell::MainMissionOverrideObservation& missions) {
    model.main_missions_available = missions.prepared;
    model.main_missions_override_active = missions.active;
    model.main_missions_authored_offline_route = missions.authored_offline_route;
    model.main_mission_claim_leases =
        static_cast<std::uint32_t>(missions.claimed_main_mission_count);
    model.progression_unlock_claim_leases =
        static_cast<std::uint32_t>(missions.claimed_progression_unlock_count);
    model.onboarding_dependency_claim_leases =
        static_cast<std::uint32_t>(missions.claimed_onboarding_dependency_count);
    model.mission_pending_restore =
        static_cast<std::uint32_t>(missions.pending_restore_count);
    model.main_mission_status = missions.detail;
}

void merge_progression_provider_observations(
    starfell::overlay::OfflineFeatureModel& model,
    const starfell::FixedStopEntitlementProviderObservation& fixed_stops,
    const starfell::NeighborhoodUnlockOverrideObservation& neighborhoods,
    bool fixed_stop_route_ready) {
    if (!model.status.empty()) model.status += '\n';
    model.status += "Fixed-stop provider: ";
    if (!fixed_stops.prepared || !fixed_stop_route_ready) {
        model.status += "unavailable";
    } else {
        model.status += fixed_stops.enabled ? "active" : "native";
        model.status += " | completions=" + std::to_string(fixed_stops.completions);
        model.status += " | mixed=" + std::to_string(fixed_stops.mixed_completions);
    }
    model.status += "\nNeighborhood updater: ";
    if (!neighborhoods.prepared) {
        model.status += "unavailable";
    } else {
        model.status += neighborhoods.enabled ? "active" : "native";
        model.status += " | matches=" + std::to_string(neighborhoods.target_matches);
        model.status += " | overrides=" + std::to_string(neighborhoods.overrides);
    }
}
void record(const std::string& json) {
    starfell::logging::event(starfell::logging::Channel::runtime, json);
}
void console_line(std::string_view text) {
    const bool error = text.starts_with("error: ");
    starfell::logging::write(error ? starfell::logging::Level::error : starfell::logging::Level::info,
        starfell::logging::Channel::command, error ? text.substr(7) : text);
}
void activity_line(starfell::ConsoleSource source, std::string_view text,
                   starfell::ConsoleSeverity severity) {
    starfell::logging::write(severity, source, text);
}
bool equals(std::string a, std::string b) {
    for (auto* text : {&a, &b}) for (auto& c : *text) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + 32);
    return a == b;
}
}
