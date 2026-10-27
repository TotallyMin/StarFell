add_library(starfell_steam_restart_guard STATIC
    Extension/Boot/steam_restart_guard.cpp
    Extension/Boot/offline_steam.cpp)

target_link_libraries(starfell_steam_restart_guard PUBLIC starfell_launcher_support starfell_hooks starfell_logging)
add_library(starfell_startup_interventions STATIC
    Extension/Boot/startup_interventions.cpp)

add_library(starfell_fast_travel_unlock STATIC
    Extension/Progression/fast_travel_unlock.cpp)

add_library(starfell_local_profile STATIC
    Extension/Profile/local_profile.cpp
    Extension/Profile/profile_defaults.cpp
    Extension/Profile/local_profile_store.cpp
    Extension/Profile/profile_update.cpp
    Extension/Profile/database_schema.cpp
    Extension/Profile/database_codec.cpp
    Extension/Objects/placement_database.cpp
    Extension/Objects/object_categories.cpp
    Extension/Profile/local_profile_legacy.cpp
    Extension/Customization/local_customization.cpp
    Extension/News/news_feed.cpp
    Extension/Progression/rip_score.cpp
    Extension/World/profile_policy.cpp
    Extension/Rendering/profile_policy.cpp
    Extension/Settings/input_bindings.cpp
    Extension/Progression/challenge_profile.cpp
    Extension/Objects/placements_store.cpp
    Extension/Objects/ParkEditor/park_document.cpp
    Extension/Objects/ParkEditor/park_mods.cpp
)
target_link_libraries(starfell_local_profile PUBLIC starfell_fast_travel_unlock starfell_content_cache starfell_json starfell_storage starfell_mod_list)

include(cmake/ProfileData.cmake)

