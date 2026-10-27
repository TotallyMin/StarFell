add_library(starfell_runtime SHARED
    Runtime/runtime_init.cpp
    Runtime/runtime_state.cpp
    Runtime/client_tick.cpp
    Runtime/frame_timing.cpp
    Runtime/client_context.cpp
    Runtime/level_requests.cpp
    Runtime/overlay_callbacks.cpp
    Runtime/console_bridge.cpp
    Runtime/menu_entry_hook.cpp
    Engine/Game/Abi/native_data.cpp
    Extension/Console/commands.cpp
    Extension/Console/perf_commands.cpp
    Extension/Skater/console_commands.cpp
    Extension/Multiplayer/console_commands.cpp
    Extension/Multiplayer/Session/session.cpp
    Extension/Multiplayer/Session/session_view.cpp
    Extension/Multiplayer/Session/session_send.cpp
    Extension/Multiplayer/Session/session_receive.cpp
    Extension/Multiplayer/Session/session_commands.cpp
    Extension/Multiplayer/Session/session_party.cpp
    Extension/UI/NativeMenu/native_menu.cpp
    Extension/UI/NativeMenu/native_menu_rows.cpp
    Extension/UI/NativeMenu/native_menu_multiplayer.cpp
    Extension/UI/NativeMenu/native_menu_dump.cpp
    Extension/UI/NativeMenu/native_hub.cpp
    Extension/UI/NativeMenu/native_menu_data.cpp
    Extension/Multiplayer/Remote/native_skater.cpp
    Extension/Multiplayer/Remote/native_skater_spawn.cpp
    Extension/Multiplayer/Remote/puppet_cost.cpp
    Extension/Multiplayer/Remote/native_cosmetics.cpp
    Extension/Multiplayer/Remote/native_audio.cpp
    Extension/Multiplayer/Voice/voice_chat.cpp
    Extension/Multiplayer/Voice/native_voice.cpp
    Extension/Multiplayer/Hud/native_player_ui.cpp
    Extension/Multiplayer/Hud/native_party.cpp
    Extension/Multiplayer/Hud/custom_nametags.cpp
    Extension/Multiplayer/Hud/game_ui_state.cpp
    Extension/Multiplayer/Hud/follow_camera.cpp
    Extension/Multiplayer/Hud/native_party_hooks.cpp
    Extension/Multiplayer/Hud/native_party_requests.cpp
    Extension/Throwdowns/native_throwdowns.cpp
    Extension/Throwdowns/native_type_scan.cpp
    Extension/Throwdowns/throwdown_lab.cpp
    Extension/Throwdowns/throwdown_relay.cpp
    Extension/Throwdowns/throwdown_debug_text.cpp
    Extension/Throwdowns/skate_trick_rule.cpp
    Extension/Throwdowns/graph_throttle.cpp
    Extension/Throwdowns/throwdown_wire.cpp
    Extension/Multiplayer/Hud/native_indicators.cpp
    Extension/Multiplayer/Steam/steam_transport.cpp
    Extension/Multiplayer/Steam/steam_social.cpp
    Extension/Multiplayer/Steam/steam_friend_join.cpp
    Extension/Multiplayer/Steam/steam_lobbies.cpp
    Extension/Multiplayer/Steam/steam_lobby_api.cpp
    Extension/Multiplayer/Steam/steam_server_browser.cpp
    Extension/Multiplayer/Net/protocol.cpp
    Extension/Multiplayer/Remote/playback_buffers.cpp
    Extension/Multiplayer/Net/wire_codec.cpp
    Extension/Multiplayer/Net/delta_codec.cpp
    Extension/Multiplayer/Session/password.cpp
    Extension/Settings/console_commands.cpp
    Extension/Settings/job_spin.cpp
    Extension/Settings/engine_tweaks.cpp
    Extension/UI/ui_pointer_skip.cpp
    Extension/Settings/named_settings.cpp
    Extension/World/console_commands.cpp
    Extension/Rendering/console_commands.cpp
    Extension/Progression/console_commands.cpp
    Extension/Objects/console_commands.cpp
    Extension/Objects/ParkEditor/park_editor_commands.cpp
    Extension/Objects/ParkEditor/park_editor_runtime.cpp
    Extension/Objects/ParkEditor/park_editor_preview.cpp
    Extension/Objects/ParkEditor/park_editor_selection.cpp
    Extension/Objects/ParkEditor/park_editor_lobby.cpp
    Extension/Objects/ParkEditor/park_editor_operations.cpp
    Extension/Objects/network_object_runtime.cpp
    Runtime/bootstrap.cpp
    Runtime/exports.cpp
    Extension/Assets/native_patch_support.cpp
    Extension/Assets/mod_layers.cpp
    Extension/Assets/live_mods.cpp
    Extension/Assets/loose_files.cpp
    Extension/Scripting/lua_startup.cpp
    Extension/Scripting/custom_script_loader.cpp
    Extension/Assets/morph_memory_pool.cpp
    Extension/Assets/native_render_resource_pool.cpp
    Extension/World/native_entity_pages.cpp
    Extension/Rendering/display_startup.cpp
    Engine/Game/World/world_model.cpp
    Extension/World/level_loading.cpp
    Extension/World/loading_screen.cpp
    Extension/Customization/preset_lookup_guard.cpp
    Extension/Skater/skater_model.cpp
    Extension/Skater/skater_observer.cpp
    Extension/Skater/client_source_spawn.cpp
    Extension/Skater/source_spawn_camera.cpp
    Extension/Skater/client_noclip.cpp
    Extension/Skater/client_first_person.cpp
    Extension/Skater/client_debug.cpp
    Extension/Skater/ai_skaters.cpp
    Extension/Skater/no_bail.cpp
    Extension/Skater/physics_tuning.cpp
    Extension/Multiplayer/Remote/remote_collision.cpp
    Extension/Skater/physics_tuning_model.cpp
    Extension/Skater/offboard_flight.cpp
    Extension/Skater/camera_observer.cpp
    Extension/Boot/offline_boot.cpp
    Extension/Progression/progression_service_guard.cpp
    Extension/Progression/mission_progression_override.cpp
    Extension/Progression/mission_progression_observation.cpp
    Extension/Settings/gameplay_settings_override.cpp
    Extension/Settings/gameplay_settings_leases.cpp
    Extension/Settings/gameplay_settings_model.cpp
    Extension/Skater/skater_slot_override.cpp
    Extension/Rendering/graphics_labels.cpp
    Extension/Profile/local_profile_runtime.cpp
    Extension/Profile/local_profile_world.cpp
    Extension/Profile/local_profile_preferences.cpp
    Extension/Profile/local_profile_objects.cpp
    Extension/Profile/local_profile_missions.cpp
    Extension/Customization/local_cosmetic_catalog.cpp
    Extension/Customization/local_customization_runtime.cpp
    Extension/Progression/local_entitlement_trigger_runtime.cpp
    Extension/Progression/local_neighborhood_runtime.cpp
    Extension/Customization/local_player_card_runtime.cpp
    Extension/News/local_news_runtime.cpp
    Extension/News/live_news.cpp
    Extension/Objects/local_object_runtime.cpp
    Extension/Music/local_music_assets.cpp
    Extension/Music/local_music_ui.cpp
    Extension/Objects/local_buildkit_labels.cpp
    Extension/Objects/local_buildkit_limits.cpp
    Extension/Progression/local_rip_score_runtime.cpp
    Extension/Progression/local_challenge_trace.cpp
    Extension/Progression/script_error_log.cpp
    Extension/Progression/board_wear.cpp
    Extension/Progression/local_challenge_runtime.cpp
    Extension/Settings/local_user_settings.cpp
    Extension/Settings/local_gameplay_settings.cpp
    Extension/Settings/local_native_settings.cpp
    Extension/Travel/local_location_travel.cpp
    Extension/World/local_park_rotation.cpp
    Extension/World/local_world_layers.cpp
    Extension/World/visual_environment.cpp
    Extension/World/local_population_controls.cpp
    Extension/World/native_route_lookahead.cpp
    Extension/World/local_world_controls.cpp
    Extension/World/local_atmosphere_controls.cpp
    Extension/Rendering/local_graphics_controls.cpp
    Extension/Objects/local_placements_runtime.cpp
    Extension/Progression/local_progression_runtime.cpp
    Extension/Progression/entitlement_request_hook.cpp
    Extension/Progression/entitlement_request_parse.cpp
    Extension/Progression/fixed_stop_entitlement_provider.cpp
    Extension/Progression/fixed_stop_native_result.cpp
    Extension/Progression/fixed_stop_entitlement_observation.cpp
    Extension/Progression/neighborhood_unlock_override.cpp
    Extension/Boot/user_data_redirect.cpp
    Extension/Boot/ea_app_block.cpp)

target_link_libraries(starfell_runtime PRIVATE starfell_logging starfell_profiler starfell_supported_build starfell_overlay starfell_console_core starfell_steam_restart_guard
    starfell_startup_interventions starfell_fast_travel_unlock starfell_local_profile starfell_initfs starfell_mods starfell_startup_window starfell_custom_scripts starfell_world_layer_scan bcrypt
    starfell_https winhttp ws2_32 xaudio2 ole32 starfell_game_archives starfell_word_filter)

add_library(starfell_custom_level_manifest STATIC
    Engine/Game/World/custom_level_manifest.cpp)
target_link_libraries(starfell_custom_level_manifest PUBLIC starfell_json)
target_link_libraries(starfell_runtime PRIVATE starfell_custom_level_manifest)

set_target_properties(starfell_runtime PROPERTIES OUTPUT_NAME "StarFell" PREFIX "")
starfell_version_info(starfell_runtime "StarFell mod runtime" "StarFell.dll" VFT_DLL)
target_include_directories(starfell_runtime SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/External/steam_networking")
target_link_libraries(starfell_runtime PRIVATE starfell_lz4 starfell_zstd)

# Menu fonts shared with the launcher (licences in External/fonts).
set(starfell_menu_fonts "${PROJECT_SOURCE_DIR}/External/fonts")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/menu_fonts.rc"
    "FONT_HEADING RCDATA \"${starfell_menu_fonts}/Montserrat-ExtraBold.ttf\"\n"
    "FONT_BODY RCDATA \"${starfell_menu_fonts}/Montserrat-SemiBold.ttf\"\n"
    "FONT_BRUSH RCDATA \"${starfell_menu_fonts}/PermanentMarker-Regular.ttf\"\n")
target_sources(starfell_runtime PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/menu_fonts.rc")

# The startup window's picture (PNG or JPEG), built into StarFell.dll when the
# repository has one. StarFell.Splash.png/.jpg beside the DLL still takes
# precedence at run time.
set(starfell_startup_art "${PROJECT_SOURCE_DIR}/assets/startup_splash.png")
if(NOT EXISTS "${starfell_startup_art}")
    set(starfell_startup_art "${PROJECT_SOURCE_DIR}/assets/startup_splash.jpg")
endif()
if(EXISTS "${PROJECT_SOURCE_DIR}/assets")
    # Adding or removing the picture re-runs the configure step.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/assets")
endif()
# Chat emotes (Better Chat's pack format; Extension/UI/Overlay/chat_emotes.h), built into
# StarFell.dll when the repository has assets/emotes/emotes.json and emotes.png.
set(starfell_emotes "${PROJECT_SOURCE_DIR}/assets/emotes")
if(EXISTS "${starfell_emotes}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${starfell_emotes}")
endif()
if(EXISTS "${starfell_emotes}/emotes.json" AND EXISTS "${starfell_emotes}/emotes.png")
    file(TO_NATIVE_PATH "${starfell_emotes}" starfell_emotes_native)
    string(REPLACE "\\" "\\\\" starfell_emotes_native "${starfell_emotes_native}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/emotes.rc"
        "EMOTES_JSON RCDATA \"${starfell_emotes_native}\\\\emotes.json\"\n"
        "EMOTES_PNG RCDATA \"${starfell_emotes_native}\\\\emotes.png\"\n")
    target_sources(starfell_runtime PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/emotes.rc")
endif()
if(EXISTS "${starfell_startup_art}")
    file(TO_NATIVE_PATH "${starfell_startup_art}" starfell_startup_art_native)
    string(REPLACE "\\" "\\\\" starfell_startup_art_native "${starfell_startup_art_native}")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/startup_art.rc"
        "STARTUP_SPLASH RCDATA \"${starfell_startup_art_native}\"\n")
    target_sources(starfell_runtime PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/startup_art.rc")
endif()
