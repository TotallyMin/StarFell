add_executable(starfell_launcher WIN32 Launcher/main.cpp Launcher/launch.cpp
    Launcher/gui.cpp Launcher/gui_launcher.cpp Launcher/gui_renderer.cpp Launcher/gui_home.cpp
    Launcher/gui_settings.cpp Launcher/gui_sign_in.cpp Launcher/gui_mods.cpp Launcher/gui_mods_browse.cpp
    Launcher/updater.cpp Launcher/mod_manager.cpp Launcher/thunderstore.cpp)
target_link_libraries(starfell_launcher PRIVATE starfell_logging starfell_content_cache_install starfell_world_layer_scan starfell_launcher_support starfell_initfs
    starfell_mod_list starfell_json starfell_miniz starfell_imgui winhttp shell32 dwmapi windowscodecs ole32)
set_target_properties(starfell_launcher PROPERTIES OUTPUT_NAME "StarFellLauncher")
starfell_version_info(starfell_launcher "StarFell launcher" "StarFellLauncher.exe" VFT_APP)

option(STARFELL_BUILD_LAUNCHER_TESTS "Build launcher mod manager regression tests" OFF)
if(STARFELL_BUILD_LAUNCHER_TESTS)
    enable_testing()
    add_executable(starfell_mod_manager_tests Launcher/Test/mod_manager_tests.cpp Launcher/mod_manager.cpp)
    target_link_libraries(starfell_mod_manager_tests PRIVATE starfell_mod_list starfell_launcher_support starfell_miniz shell32)
    add_test(NAME launcher_mod_manager COMMAND starfell_mod_manager_tests)
    add_executable(starfell_thunderstore_tests Launcher/Test/thunderstore_tests.cpp Launcher/thunderstore.cpp)
    target_link_libraries(starfell_thunderstore_tests PRIVATE starfell_json starfell_miniz)
    target_include_directories(starfell_thunderstore_tests PRIVATE "${PROJECT_SOURCE_DIR}")
    add_test(NAME launcher_thunderstore COMMAND starfell_thunderstore_tests)
    add_executable(starfell_content_catalogs_tests Engine/Vfs/Test/content_catalogs_tests.cpp)
    target_link_libraries(starfell_content_catalogs_tests PRIVATE starfell_content_cache)
    add_test(NAME content_catalogs COMMAND starfell_content_catalogs_tests)
    add_executable(starfell_content_cache_install_tests Engine/Vfs/Test/content_cache_install_tests.cpp)
    target_link_libraries(starfell_content_cache_install_tests PRIVATE starfell_content_cache_install)
    add_test(NAME content_cache_install COMMAND starfell_content_cache_install_tests)
    add_executable(starfell_item_thumbnails_tests Engine/Vfs/Test/item_thumbnails_tests.cpp)
    target_link_libraries(starfell_item_thumbnails_tests PRIVATE starfell_game_archives)
    add_test(NAME item_thumbnails COMMAND starfell_item_thumbnails_tests "${STARFELL_TEST_GAME_ROOT}")
    add_executable(starfell_world_layer_scan_tests Engine/Vfs/Test/world_layer_scan_tests.cpp)
    target_link_libraries(starfell_world_layer_scan_tests PRIVATE starfell_world_layer_scan)
    add_test(NAME world_layer_scan COMMAND starfell_world_layer_scan_tests "${STARFELL_TEST_GAME_ROOT}")
    add_executable(starfell_mod_merge_grid_tests Engine/Vfs/Test/mod_merge_grid_tests.cpp)
    target_link_libraries(starfell_mod_merge_grid_tests PRIVATE starfell_mods)
    add_test(NAME mod_merge_grid COMMAND starfell_mod_merge_grid_tests "${STARFELL_TEST_GAME_ROOT}")
    add_executable(starfell_mod_build_stamp_tests Engine/Vfs/Test/mod_build_stamp_tests.cpp)
    target_link_libraries(starfell_mod_build_stamp_tests PRIVATE starfell_mods)
    target_include_directories(starfell_mod_build_stamp_tests PRIVATE "${PROJECT_SOURCE_DIR}")
    add_test(NAME mod_build_stamp COMMAND starfell_mod_build_stamp_tests)
    add_executable(starfell_bundle_ref_table_tests Engine/Vfs/Test/bundle_ref_table_tests.cpp)
    target_link_libraries(starfell_bundle_ref_table_tests PRIVATE starfell_mods)
    add_test(NAME bundle_ref_table COMMAND starfell_bundle_ref_table_tests)
    add_executable(starfell_mod_scoring_tests Engine/Vfs/Test/mod_scoring_tests.cpp)
    target_link_libraries(starfell_mod_scoring_tests PRIVATE starfell_mods)
    add_test(NAME mod_scoring COMMAND starfell_mod_scoring_tests "${STARFELL_TEST_GAME_ROOT}")
endif()

# Launcher fonts (OFL / Apache, licences beside them) and the background photo,
# embedded when present. A StarFellLauncher.background.jpg beside the exe
# overrides the photo at runtime.
set(launcher_fonts "${PROJECT_SOURCE_DIR}/External/fonts")
set(launcher_resources
    "FONT_HEADING RCDATA \"${launcher_fonts}/Montserrat-ExtraBold.ttf\"\n"
    "FONT_BODY RCDATA \"${launcher_fonts}/Montserrat-SemiBold.ttf\"\n"
    "FONT_BRUSH RCDATA \"${launcher_fonts}/PermanentMarker-Regular.ttf\"\n")
set(launcher_background "${PROJECT_SOURCE_DIR}/assets/launcher/background.jpg")
if(EXISTS "${launcher_background}")
    list(APPEND launcher_resources "LAUNCHER_BACKGROUND RCDATA \"${launcher_background}\"\n")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${launcher_background}")
endif()
# Tile icons: the skate tool on MODS, the wheel on SETTINGS.
foreach(tile_icon mods settings)
    set(tile_icon_path "${PROJECT_SOURCE_DIR}/assets/launcher/icon_${tile_icon}.png")
    if(EXISTS "${tile_icon_path}")
        string(TOUPPER "${tile_icon}" tile_icon_name)
        list(APPEND launcher_resources "LAUNCHER_ICON_${tile_icon_name} RCDATA \"${tile_icon_path}\"\n")
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${tile_icon_path}")
    endif()
endforeach()
# Icon resource 1: the exe's icon in Explorer and the window's icon.
set(launcher_icon "${PROJECT_SOURCE_DIR}/assets/launcher/icon.ico")
if(EXISTS "${launcher_icon}")
    list(PREPEND launcher_resources "1 ICON \"${launcher_icon}\"\n")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${launcher_icon}")
endif()
string(CONCAT launcher_resources ${launcher_resources})
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generated/launcher_resources.rc" "${launcher_resources}")
target_sources(starfell_launcher PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated/launcher_resources.rc")

# Off for local builds so a deployed development DLL is never replaced by a release.
option(STARFELL_LAUNCHER_AUTO_UPDATE "Let the launcher replace itself and StarFell.dll from the launcher config" OFF)

# Updates come from the public GitHub releases of STARFELL_RELEASE_REPO: the launcher and
# server read launcher.json (game depot/manifest and the pinned downloads) from the latest
# release. No credentials are built in. Build folders configured before releases moved
# still cache the old private repo, which no public build can read.
if(STARFELL_RELEASE_REPO STREQUAL "TotallyMin/StarFell")
    unset(STARFELL_RELEASE_REPO CACHE)
endif()
set(STARFELL_RELEASE_REPO "TotallyMin/StarFell" CACHE STRING "Public GitHub owner/repo whose releases update the launcher")
set(launcher_release_repo "${STARFELL_RELEASE_REPO}")
if(NOT launcher_release_repo MATCHES "^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$")
    message(FATAL_ERROR "STARFELL_RELEASE_REPO must be owner/repo")
endif()
if(STARFELL_LAUNCHER_AUTO_UPDATE)
    set(launcher_binary_updates true)
else()
    set(launcher_binary_updates false)
endif()
configure_file(cmake/templates/launcher_update_config.h.in generated/launcher_update_config.h @ONLY)

# StarFellEmotePacker: a folder of pictures and GIFs -> the chat emote pack (emotes.json +
# emotes.png) that StarFell.dll bakes in from assets/emotes.
add_executable(starfell_emote_packer EmotePacker/main.cpp)
target_link_libraries(starfell_emote_packer PRIVATE starfell_json windowscodecs ole32)
set_target_properties(starfell_emote_packer PROPERTIES OUTPUT_NAME "StarFellEmotePacker")

# StarFell dedicated server: a headless session host. It runs from its own folder
# next to steam_api64.dll and the Steam client files; no game install needed.
add_executable(starfell_server Server/main.cpp Server/server_host.cpp Server/server_party.cpp
    Extension/Multiplayer/Session/party_book.cpp Server/server_config.cpp Server/steam_server.cpp
    Server/server_update.cpp Launcher/updater.cpp
    Extension/Multiplayer/Steam/steam_transport.cpp Extension/Multiplayer/Net/protocol.cpp
    Extension/Multiplayer/Net/delta_codec.cpp Extension/Multiplayer/Net/wire_codec.cpp
    Extension/Multiplayer/Remote/playback_buffers.cpp Extension/Multiplayer/Session/password.cpp
    Server/server_activity.cpp Server/server_votes.cpp Extension/Throwdowns/throwdown_wire.cpp)
target_include_directories(starfell_server SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/External/steam_networking")
target_link_libraries(starfell_server PRIVATE starfell_launcher_support starfell_world_layer_scan starfell_json
    starfell_lz4 starfell_zstd starfell_logging starfell_miniz starfell_word_filter winhttp bcrypt winmm)
set_target_properties(starfell_server PROPERTIES OUTPUT_NAME "StarFellServer")
starfell_version_info(starfell_server "StarFell dedicated server" "StarFellServer.exe" VFT_APP)
