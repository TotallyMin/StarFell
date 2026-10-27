add_library(starfell_overlay STATIC

    Extension/UI/Overlay/overlay.cpp
    Extension/UI/Overlay/overlay_render.cpp
    Extension/UI/Overlay/overlay_console.cpp
    Extension/UI/Overlay/overlay_input.cpp
    Extension/UI/Overlay/overlay_notices.cpp
    Extension/UI/Overlay/chat_overlay.cpp
    Extension/UI/Overlay/game_text_overlay.cpp
    Extension/UI/Overlay/perf_overlay.cpp
    Extension/UI/Overlay/skate_hud_overlay.cpp
    Extension/UI/Overlay/nametag_overlay.cpp
    Extension/UI/Overlay/chat_emotes.cpp
    Extension/UI/Overlay/console_suggestions.cpp
    Extension/UI/Overlay/skate_menu.cpp
    Extension/UI/Overlay/skate_menu_world.cpp
    Extension/UI/Overlay/skate_menu_skater.cpp
    Extension/UI/Overlay/skate_menu_settings.cpp
    Extension/UI/Overlay/park_editor.cpp
    Extension/UI/Overlay/park_editor_actions.cpp
    Extension/UI/Overlay/park_editor_panels.cpp
    Extension/UI/Overlay/park_editor_viewport.cpp
    Extension/UI/Overlay/park_previews.cpp
    Extension/UI/Overlay/gpu_diagnostics.cpp
    Extension/UI/Overlay/input_capture.cpp
    Extension/UI/Overlay/playstation_input.cpp
    Extension/UI/Overlay/cursor.cpp
    Extension/UI/Overlay/progression_menu.cpp
    Extension/UI/Overlay/atmosphere_menu.cpp
    Extension/UI/Overlay/modding_menu.cpp
    Extension/UI/Overlay/multiplayer_menu.cpp
    Extension/UI/Overlay/multiplayer_lobbies.cpp
    Extension/UI/Overlay/multiplayer_session.cpp
)
target_link_libraries(starfell_overlay PUBLIC starfell_logging starfell_profiler starfell_imgui starfell_hooks starfell_console_core dxguid PRIVATE hid cfgmgr32 shell32 starfell_initfs starfell_custom_scripts starfell_game_archives starfell_mods starfell_json windowscodecs ole32)

# The window shown from the moment StarFell loads until the game's own window appears.
add_library(starfell_startup_window STATIC Extension/UI/Startup/startup_window.cpp)
target_link_libraries(starfell_startup_window PRIVATE gdi32 user32 msimg32 shell32 ole32 windowscodecs)
