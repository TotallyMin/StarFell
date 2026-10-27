#include "gui_internal.h"

#include "mod_manager.h"

#include <cmath>
#include <format>

namespace starfell::launcher_gui::detail {
namespace {

// ---------------------------------------------------------------- background

void draw_background(ImDrawList* draw, ImVec2 size, float time) {
    struct Glow { ImVec2 centre; float radius; ImVec4 colour; };
    const Glow glows[]{
        {ImVec2(size.x * 0.95f, size.y * 1.05f), S(720), ImVec4(1.0f, 0.55f, 0.12f, 0.32f)},
        {ImVec2(size.x * 0.01f, size.y * -0.06f), S(540), ImVec4(0.18f, 0.34f, 0.9f, 0.20f)},
    };
    const auto top = ImGui::ColorConvertU32ToFloat4(color::background_top);
    const auto bottom = ImGui::ColorConvertU32ToFloat4(color::background_bottom);
    const auto field = [&](float x, float y) {
        const float v = std::clamp(y / size.y, 0.0f, 1.0f);
        ImVec4 out(top.x + (bottom.x - top.x) * v, top.y + (bottom.y - top.y) * v, top.z + (bottom.z - top.z) * v, 1);
        for (const auto& glow : glows) {
            const float t = std::min(std::hypot(x - glow.centre.x, y - glow.centre.y) / glow.radius, 1.0f);
            const float a = glow.colour.w * (1 - t) * (1 - t);
            out.x += (glow.colour.x - out.x) * a;
            out.y += (glow.colour.y - out.y) * a;
            out.z += (glow.colour.z - out.z) * a;
        }
        return ImGui::ColorConvertFloat4ToU32(out);
    };
    constexpr int columns = 32, rows = 18;
    const float cell_x = size.x / columns, cell_y = size.y / rows;
    for (int row = 0; row < rows; ++row)
        for (int column = 0; column < columns; ++column) {
            const float x0 = cell_x * static_cast<float>(column), y0 = cell_y * static_cast<float>(row);
            const float x1 = x0 + cell_x, y1 = y0 + cell_y;
            draw->AddRectFilledMultiColor(ImVec2(x0, y0), ImVec2(x1, y1),
                field(x0, y0), field(x1, y0), field(x1, y1), field(x0, y1));
        }

    // Drifting chalk-outline shapes.
    for (int index = 0; index < 26; ++index) {
        const float seed = static_cast<float>(index) * 12.9898f;
        const float fx = std::fmod(std::abs(std::sin(seed) * 43758.5453f), 1.0f);
        const float fy = std::fmod(std::abs(std::sin(seed * 1.7f) * 24634.6345f), 1.0f);
        const float drift = time * (0.004f + 0.006f * fx);
        const ImVec2 centre(size.x * std::fmod(fx + drift, 1.0f),
                            size.y * fy + std::sin(time * 0.3f + seed) * S(6));
        const float radius = S(3.0f + 16.0f * fy * fx);
        const float alpha = 0.05f + 0.07f * fy;
        if (index % 3 == 0) {
            const float angle = time * 0.2f + seed;
            ImVec2 corners[4];
            for (int corner = 0; corner < 4; ++corner) {
                const float a = angle + static_cast<float>(corner) * 1.5707963f;
                corners[corner] = ImVec2(centre.x + std::cos(a) * radius, centre.y + std::sin(a) * radius);
            }
            draw->AddQuad(corners[0], corners[1], corners[2], corners[3], rgba(255, 255, 255, alpha), 1.2f);
        } else {
            draw->AddCircle(centre, radius, rgba(255, 255, 255, alpha), 0, 1.2f);
        }
    }
    draw->AddRectFilledMultiColor(ImVec2(0, size.y * 0.82f), size, 0, 0, rgba(0, 0, 0, 0.58f),
                                  rgba(0, 0, 0, 0.58f));
}

void draw_photo(ImDrawList* draw, ImVec2 size, float time) {
    const float zoom = 1.04f + 0.02f * std::sin(time * 0.05f);
    const float scale = std::max(size.x / g_background.width, size.y / g_background.height) * zoom;
    const ImVec2 image(g_background.width * scale, g_background.height * scale);
    const ImVec2 origin((size.x - image.x) * 0.5f + std::sin(time * 0.037f) * S(10),
                        (size.y - image.y) * 0.5f + std::cos(time * 0.029f) * S(6));
    draw->AddImage(g_background.id, origin, ImVec2(origin.x + image.x, origin.y + image.y));
    draw->AddRectFilled(ImVec2(0, 0), size, rgba(6, 8, 12, 0.35f));
    draw->AddRectFilledMultiColor(ImVec2(0, size.y * 0.45f), size, rgba(6, 8, 12, 0), rgba(6, 8, 12, 0),
                                  rgba(6, 8, 12, 0.92f), rgba(6, 8, 12, 0.92f));
    draw->AddRectFilledMultiColor(ImVec2(0, 0), ImVec2(size.x, S(90)), rgba(6, 8, 12, 0.55f), rgba(6, 8, 12, 0.55f),
                                  rgba(6, 8, 12, 0), rgba(6, 8, 12, 0));
}

// ------------------------------------------------------------------ drawing

using skate_theme::Icon;
using skate_theme::rotate_since;
void rough_rect(ImDrawList* draw, ImVec2 a, ImVec2 b, ImU32 colour, unsigned seed) {
    skate_theme::rough_rect(draw, a, b, colour, seed, g_scale);
}
void scribble(ImDrawList* draw, ImVec2 a, ImVec2 b, unsigned seed) { skate_theme::scribble(draw, a, b, seed, g_scale); }
void progress_bar(ImDrawList* draw, ImVec2 a, ImVec2 b, float fill, float time) {
    skate_theme::striped_bar(draw, a, b, fill, time, g_scale);
}

void draw_title(ImDrawList* draw, const Fonts& fonts, ImVec2 position, const char* text) {
    const int start = draw->VtxBuffer.Size;
    const float size = fonts.title->FontSize;
    const auto extent = fonts.title->CalcTextSizeA(size, FLT_MAX, 0, text);
    draw->AddText(fonts.title, size, ImVec2(position.x + S(3), position.y + S(4)), rgba(0, 0, 0, 0.5f), text);
    draw->AddText(fonts.title, size, position, color::text, text);
    rotate_since(draw, start, -4.0f, ImVec2(position.x + extent.x * 0.5f, position.y + extent.y * 0.5f));
}

// Greeting card (top-left).
void draw_plate(ImDrawList* draw, const Fonts& fonts, ImVec2 position, const std::string& title,
                const std::string& version) {
    const float h = S(48);
    const float text = std::max(fonts.bold->CalcTextSizeA(fonts.bold->FontSize, FLT_MAX, 0, title.c_str()).x,
                                fonts.body->CalcTextSizeA(fonts.body->FontSize, FLT_MAX, 0, version.c_str()).x);
    const float width = std::max(S(300), h + S(28) + text);
    draw->AddRectFilled(ImVec2(position.x + h, position.y + S(3)), ImVec2(position.x + width, position.y + h - S(3)),
        rgba(44, 47, 49, 0.92f));
    draw->AddRect(ImVec2(position.x + h, position.y + S(3)), ImVec2(position.x + width, position.y + h - S(3)),
        color::outline, 0, 0, S(10));
    draw->AddRectFilled(position, ImVec2(position.x + h, position.y + h), color::avatar);
    const ImVec2 c(position.x + h * 0.5f, position.y + h * 0.5f);
    const int start = draw->VtxBuffer.Size;
    draw->AddRectFilled(ImVec2(c.x - S(15), c.y - S(5.5f)), ImVec2(c.x + S(15), c.y + S(5.5f)), color::text, S(5.5f));
    for (const float x : {-9.0f, 9.0f}) draw->AddCircleFilled(ImVec2(c.x + S(x), c.y), S(1.8f), color::avatar);
    rotate_since(draw, start, -40.0f, c);
    draw->AddCircleFilled(ImVec2(position.x + S(3), position.y + S(3)), S(5), color::good);
    draw->AddText(fonts.bold, fonts.bold->FontSize, ImVec2(position.x + h + S(12), position.y + S(5)), color::text,
        title.c_str());
    draw->AddText(fonts.body, fonts.body->FontSize, ImVec2(position.x + h + S(12), position.y + S(26)), color::muted,
        version.c_str());
}

// Invisible button with unique id + hover state.
bool hit(const char* id, ImVec2 position, ImVec2 size, bool enabled, bool& hovered) {
    ImGui::SetCursorScreenPos(position);
    ImGui::BeginDisabled(!enabled);
    const bool pressed = ImGui::InvisibleButton(id, size);
    hovered = enabled && ImGui::IsItemHovered();
    ImGui::EndDisabled();
    if (hovered) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    return pressed;
}

// Card body: rough tile, scribble when disabled, hover ring.
void card_body(ImDrawList* draw, ImVec2 position, ImVec2 size, bool enabled, bool hovered, unsigned seed,
               ImU32 fill) {
    const ImVec2 end(position.x + size.x, position.y + size.y);
    rough_rect(draw, position, end, fill, seed);
    if (!enabled) scribble(draw, position, end, seed);
    if (hovered)
        draw->AddRect(ImVec2(position.x - S(3), position.y - S(3)), ImVec2(end.x + S(3), end.y + S(3)), color::text, 0,
            0, S(3));
}

// Faded white icon centred in a card.
void card_icon(ImDrawList* draw, const Background& icon, ImVec2 centre, float size, bool hovered) {
    if (!icon.id) return;
    const float half = size * 0.5f;
    draw->AddImage(icon.id, ImVec2(centre.x - half, centre.y - half), ImVec2(centre.x + half, centre.y + half),
        ImVec2(0, 0), ImVec2(1, 1), rgba(255, 255, 255, hovered ? 0.65f : 0.35f));
}

// ---- Hero card: oversized primary action ----------------------------------

bool hero_card(ImDrawList* draw, const Fonts& fonts, ImVec2 position, ImVec2 size, const char* label,
               const std::string& detail, bool enabled, bool secondary) {
    bool hovered{};
    const bool pressed = hit("##hero", position, size, enabled, hovered);
    card_body(draw, position, size, enabled, hovered, 1,
              enabled && !secondary ? color::blue : enabled ? color::tile : color::tile_grey);
    const ImVec2 end(position.x + size.x, position.y + size.y);
    draw->AddText(fonts.action, fonts.action->FontSize, ImVec2(position.x + S(34), position.y + S(28)),
        enabled ? color::ink : rgba(160, 162, 168), label);
    if (!detail.empty())
        draw->AddText(fonts.bold, fonts.bold->FontSize, ImVec2(position.x + S(36), end.y - S(34) - fonts.bold->FontSize),
            enabled ? rgba(0, 0, 0, 0.62f) : color::muted, detail.c_str(), nullptr, size.x - S(170));
    const ImVec2 centre(end.x - S(60), position.y + size.y * 0.5f);
    card_icon(draw, g_icon_settings, centre, S(60), hovered);
    const ImU32 ink = rgba(255, 255, 255, hovered ? 0.6f : 0.3f);
    for (int tooth = 0; tooth < 8; ++tooth) {
        const float angle = static_cast<float>(tooth) * 0.7853982f;
        draw->AddLine(ImVec2(centre.x + std::cos(angle) * S(14), centre.y + std::sin(angle) * S(14)),
                      ImVec2(centre.x + std::cos(angle) * S(24), centre.y + std::sin(angle) * S(24)), ink, S(6));
    }
    draw->AddCircle(centre, S(17), ink, 0, S(4));
    draw->AddCircle(centre, S(7), ink, 0, S(3));
    return pressed;
}

// ---- Secondary tile: label + detail + icon --------------------------------

bool secondary_tile(ImDrawList* draw, const Fonts& fonts, ImVec2 position, ImVec2 size, const char* label,
                    const std::string& detail, bool enabled, const Background& icon, unsigned seed,
                    const char* id) {
    bool hovered{};
    const bool pressed = hit(id, position, size, enabled, hovered);
    card_body(draw, position, size, enabled, hovered, seed, color::tile);
    const float text_y = position.y + (size.y - fonts.tile->FontSize) * 0.5f - (detail.empty() ? 0 : S(8));
    draw->AddText(fonts.tile, fonts.tile->FontSize, ImVec2(position.x + S(24), text_y), color::text, label);
    if (!detail.empty())
        draw->AddText(fonts.body, fonts.body->FontSize, ImVec2(position.x + S(26), text_y + fonts.tile->FontSize + S(2)),
            color::muted, detail.c_str());
    const ImVec2 centre(position.x + size.x - S(52), position.y + size.y * 0.5f);
    card_icon(draw, icon, centre, S(54), hovered);
    return pressed;
}

// ------------------------------------------------------------------ buttons

void window_buttons(ImDrawList* draw, HWND window, ImVec2 size) {
    const ImVec2 button(S(46), S(34));
    bool hovered{};
    ImVec2 position(size.x - button.x * 2, 0);
    if (hit("##minimise", position, button, true, hovered)) ShowWindow(window, SW_MINIMIZE);
    if (hovered) draw->AddRectFilled(position, ImVec2(position.x + button.x, button.y), rgba(255, 255, 255, 0.08f));
    const ImVec2 middle(position.x + button.x * 0.5f, button.y * 0.5f);
    draw->AddLine(ImVec2(middle.x - S(5), middle.y), ImVec2(middle.x + S(5), middle.y), color::text, S(1));
    position.x += button.x;
    if (hit("##close", position, button, true, hovered)) PostMessageW(window, WM_CLOSE, 0, 0);
    if (hovered) draw->AddRectFilled(position, ImVec2(position.x + button.x, button.y), rgba(232, 17, 35));
    const ImVec2 cross(position.x + button.x * 0.5f, button.y * 0.5f);
    draw->AddLine(ImVec2(cross.x - S(5), cross.y - S(5)), ImVec2(cross.x + S(5), cross.y + S(5)), color::text, S(1));
    draw->AddLine(ImVec2(cross.x - S(5), cross.y + S(5)), ImVec2(cross.x + S(5), cross.y - S(5)), color::text, S(1));
}

// ------------------------------------------------------------ status strip

void status_strip(ImDrawList* draw, const Fonts& fonts, const State& state, ImVec2 position, float width, float time) {
    Icon now = Icon::busy;
    switch (state.phase) {
    case Phase::ready: now = Icon::check; break;
    case Phase::failed: now = Icon::fail; break;
    case Phase::update_available:
    case Phase::game_missing:
    case Phase::game_outdated: now = Icon::warning; break;
    default: break;
    }
    const float height = S(125);
    const ImVec2 end(position.x + width, position.y + height);
    rough_rect(draw, position, end, color::tile, 6);
    draw->AddText(fonts.tile, fonts.tile->FontSize, ImVec2(position.x + S(22), position.y + S(14)), color::text,
        "STATUS");
    skate_theme::status_icon(draw, ImVec2(position.x + S(38), position.y + height * 0.5f), now, time, g_scale);
    float y = position.y + S(50);
    const float text_x = position.x + S(72);
    const float text_width = width - S(96);
    draw->AddText(fonts.bold, fonts.bold->FontSize, ImVec2(text_x, y),
        now == Icon::fail ? color::danger : color::text, state.status.c_str(), nullptr, text_width);
    y += fonts.bold->FontSize + S(6);
    if (!state.detail.empty()) {
        draw->AddText(fonts.body, fonts.body->FontSize, ImVec2(text_x, y), color::muted, state.detail.c_str(), nullptr,
            text_width);
        y += fonts.body->FontSize + S(8);
    }
    if (state.progress >= 0)
        progress_bar(draw, ImVec2(text_x, y), ImVec2(end.x - S(22), y + S(14)), state.progress, time);
}

} // namespace

// ------------------------------------------------------------------- frame

void frame(Launcher& launcher, const Fonts& fonts, HWND window, Ui& ui, ModsPanel& mods_panel) {
    const auto& io = ImGui::GetIO();
    const ImVec2 size = io.DisplaySize;
    const float time = static_cast<float>(ImGui::GetTime());
    const auto state = launcher.snapshot();
    const bool qr_open = !state.qr.empty();
    collect_listing(mods_panel);
    refresh_listing(mods_panel, time);
    {
        std::lock_guard lock(g_dropped_mutex);
        if (!g_dropped.empty() && !ui.settings && !ui.sign_in && !qr_open && !state.prompt && !mods_panel.installing) {
            ui.mods = true;
            mods_panel.tab = 0;
            if (!mods_panel.scanned) scan(mods_panel, launcher.session());
            start_install(mods_panel, g_dropped.front(), false);
        }
        g_dropped.clear();
    }
    const bool modal = ui.settings || ui.mods || ui.sign_in || qr_open || state.prompt.has_value();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("##root", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar(2);
    auto* draw = ImGui::GetWindowDrawList();
    if (g_background.id) draw_photo(draw, size, time);
    else draw_background(draw, size, time);

    std::string version = "Development build";
    if (update::binary_updates_enabled() && state.config && !state.config->launcher.version.empty())
        version = "Launcher " + state.config->launcher.version;
    if (time - ui.steam_checked > 5) {
        ui.steam_name = launcher_app::steam_persona_name();
        ui.steam_checked = time;
    }
    const bool greet = !launcher.settings().offline && !ui.steam_name.empty();

    // Greeting card — top-left.
    draw_plate(draw, fonts, ImVec2(S(50), S(40)), greet ? "Welcome, " + ui.steam_name : std::string("StarFell"),
               version);
    // Brushed title beneath the plate.
    draw_title(draw, fonts, ImVec2(S(52), S(100)), "RESKATE");
    window_buttons(draw, window, size);

    ImGui::BeginDisabled(modal);

    // -------------------------------------------------------- hero card
    const float hero_width = size.x - S(120);
    const ImVec2 hero_size(hero_width, S(200));
    const ImVec2 hero_pos(S(60), S(200));
    const char* label = "PLAY";
    std::string detail;
    bool enabled = !launcher.busy();
    bool secondary = false;
    const auto build = state.config && !state.config->game.build_id.empty() ? state.config->game.build_id : std::string();
    switch (state.phase) {
    case Phase::checking: label = "CHECKING"; enabled = false; break;
    case Phase::update_available: label = "UPDATE"; detail = "New StarFell files are ready"; break;
    case Phase::updating: label = "UPDATING"; enabled = false; break;
    case Phase::game_missing: label = "INSTALL"; detail = "Download skate. from Steam"; break;
    case Phase::game_outdated: label = "DOWNLOAD"; detail = "Get skate. build " + build; break;
    case Phase::downloading:
        label = "CANCEL"; enabled = true; secondary = true;
        detail = state.progress >= 0 ? std::format("Downloading  {:.0f}%", state.progress * 100) : "Downloading";
        break;
    case Phase::ready:
        label = "PLAY";
        detail = !state.config ? std::string("Offline")
               : !update::binary_updates_enabled() ? std::string("StarFell development build")
               : state.config->runtime.version.empty() ? std::string("StarFell")
               : "StarFell " + state.config->runtime.version;
        break;
    case Phase::launching: label = "LAUNCHING"; enabled = false; break;
    case Phase::failed: label = "RETRY"; break;
    }
    const auto act = [&] {
        switch (state.phase) {
        case Phase::update_available: launcher.apply_updates(); break;
        case Phase::game_missing:
        case Phase::game_outdated: open_sign_in(launcher, ui, false); break;
        case Phase::downloading: launcher.cancel(); break;
        case Phase::ready: launcher.play(); break;
        case Phase::failed: launcher.check(); break;
        default: break;
        }
    };
    if (hero_card(draw, fonts, hero_pos, hero_size, label, detail, enabled && !modal, secondary)) act();

    // --------------------------------------------------- action row: MODS / SETTINGS
    const float row_y = S(430);
    const float tile_width = (size.x - S(170)) * 0.5f;
    const ImVec2 tile_size(tile_width, S(120));
    const ImVec2 mods_pos(S(60), row_y);
    const ImVec2 settings_pos(S(60) + tile_width + S(50), row_y);
    if (time - ui.mods_checked > 5 && !ui.mods) {
        std::size_t enabled_mods = 0, installed = 0;
        const auto list = mods::scan_mods(launcher_mods::mods_root(launcher.session().paths.directory).parent_path());
        for (const auto& entry : list.entries) {
            ++installed;
            if (entry.enabled) ++enabled_mods;
        }
        const auto pending = updates(mods_panel.store, installed_versions(list)).size();
        ui.mods_detail = pending ? (pending == 1 ? std::string("1 update available")
                                  : std::format("{} updates available", pending))
                       : installed ? std::format("{} of {} enabled", enabled_mods, installed)
                       : std::string("Install and browse mods");
        ui.mods_checked = time;
    }
    if (secondary_tile(draw, fonts, mods_pos, tile_size, "MODS", ui.mods_detail, !modal, g_icon_mods, 2,
                       "##mods")) {
        ui.mods = true;
        mods_panel.scanned = false;
    }
    if (secondary_tile(draw, fonts, settings_pos, tile_size, "SETTINGS", std::string(), !modal, g_icon_settings, 3,
                       "##settings"))
        ui.settings = true;

    if (enabled && !secondary && !modal &&
        (ImGui::IsKeyPressed(ImGuiKey_Enter, false) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false))) act();
    ImGui::EndDisabled();

    // ------------------------------------------------------- status strip (bottom)
    status_strip(draw, fonts, state, ImVec2(S(60), size.y - S(180)), hero_width, time);

    if (modal) draw->AddRectFilled(ImVec2(0, 0), size, rgba(4, 6, 9, 0.72f));
    g_drag_allowed = !modal && !ImGui::IsAnyItemHovered();
    ImGui::End();

    if (state.prompt) prompt_window(launcher, fonts, size, *state.prompt, ui);
    else if (qr_open) qr_window(launcher, fonts, size, state.qr);
    else if (ui.sign_in) sign_in_window(launcher, fonts, size, ui);
    else if (ui.settings) settings_window(launcher, fonts, size, ui);
    else if (ui.mods) mods_window(launcher, fonts, size, ui, mods_panel, window);
}

} // namespace starfell::launcher_gui::detail
