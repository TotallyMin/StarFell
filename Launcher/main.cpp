#include "Engine/Core/Log/logging.h"
#include "crash_reporter.h"
#include "gui.h"
#include "launch.h"

#include <Windows.h>
#include <shellapi.h>

#include <algorithm>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace {

namespace app = starfell::launcher_app;
namespace update = starfell::launcher_update;
using starfell::logging::Channel;
using starfell::logging::Level;

std::wstring widen(std::string_view value) {
    if (value.empty()) return {};
    auto length = MultiByteToWideChar(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0);
    if (length <= 0) return L"Unknown error";
    std::wstring output(static_cast<std::size_t>(length), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), output.data(), length);
    return output;
}

// Removes a launcher-only flag so it never reaches Skate.exe.
bool take_flag(std::vector<std::wstring>& arguments, std::wstring_view flag) {
    const auto found = std::find(arguments.begin(), arguments.end(), flag);
    if (found == arguments.end()) return false;
    arguments.erase(found);
    return true;
}

// Scripted launches (`--no-gui`): update, validate and start Skate without a window.
// Returns true when the launcher replaced itself and restarted.
bool apply_headless_updates(const app::Session& session, const update::Config& config,
                            const std::vector<std::wstring>& original, bool relaunched) {
    if (!update::binary_updates_enabled()) return false;
    if (!config.runtime.url.empty() && !update::file_matches(session.paths.dll, config.runtime)) {
        try {
            update::replace_file(session.paths.dll, config.runtime);
            starfell::logging::log(Level::info, Channel::launcher, "StarFell.dll updated to {}.", config.runtime.version);
        } catch (const std::exception& exception) {
            starfell::logging::log(Level::warning, Channel::launcher, "StarFell.dll update failed: {}", exception.what());
        }
    }
    if (relaunched || config.launcher.url.empty() || update::file_matches(session.self, config.launcher)) return false;
    try {
        update::replace_running_launcher(session.self, config.launcher);
        starfell::logging::log(Level::info, Channel::launcher, "Launcher updated to {}; restarting.", config.launcher.version);
        auto arguments = original;
        arguments.emplace_back(L"--starfell-updated");
        app::relaunch(session.self, arguments);
        return true;
    } catch (const std::exception& exception) {
        starfell::logging::log(Level::warning, Channel::launcher, "Launcher update failed: {}", exception.what());
        return false;
    }
}

void run_headless(const app::Session& session, std::vector<std::wstring> arguments,
                  const std::vector<std::wstring>& original) {
    const bool relaunched = take_flag(arguments, L"--starfell-updated");
    const bool updates = !take_flag(arguments, L"--no-update") && starfell::launcher_gui::updates_enabled(session);
    const auto options = starfell::launcher::parse_launch_options(arguments);
    update::remove_previous_launcher(session.self);
    if (updates) {
        const auto config = update::fetch_config();
        if (config && apply_headless_updates(session, *config, original, relaunched)) return;
    }
    if (!app::game_files_supported(session.paths))
        throw std::runtime_error("Skate.exe is missing or is not the supported build. "
            "Open StarFellLauncher.exe without --no-gui to download it from Steam.");
    app::start_game(session, options);
}

} // namespace

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    int argc{};
    auto** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        MessageBoxW(nullptr, L"Could not read the launcher command line.",
            L"StarFell Launcher", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        return 1;
    }
    if (argc > 1 && std::wstring_view(argv[1]) == L"--starfell-crash-helper") {
        const auto result = starfell::backtrace::run_reporter(argc, argv);
        LocalFree(argv);
        return result;
    }
    const std::vector<std::wstring> original(argv + 1, argv + argc);
    LocalFree(argv);
    auto arguments = original;
    const bool headless = take_flag(arguments, L"--no-gui");

    int result = 0;
    fs::path logs;
    try {
        std::string log_level = "info";
        if (headless) log_level = starfell::launcher::parse_launch_options(
            [&] { auto copy = arguments; take_flag(copy, L"--no-update"); take_flag(copy, L"--starfell-updated"); return copy; }()).log_level;
        starfell::launcher_gui::apply_crash_report_setting();
        const auto session = app::open_session(log_level);
        logs = session.paths.logs;
        if (headless) run_headless(session, arguments, original);
        else result = starfell::launcher_gui::run(session, arguments);
    } catch (const std::exception& exception) {
        const auto message = widen(exception.what());
        starfell::logging::write(Level::critical, Channel::launcher, message);
        starfell::logging::flush();
        std::wstring dialog = L"Could not start Skate.\n\n" + message;
        if (!logs.empty()) dialog += L"\n\nDetails: " + (logs / L"StarFell.log").wstring();
        MessageBoxW(nullptr, dialog.c_str(), L"StarFell Launcher", MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        result = 1;
    }
    starfell::logging::shutdown();
    return result;
}
