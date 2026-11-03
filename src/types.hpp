#pragma once

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/shared/workspace/WorkspaceRule.hpp>
#include <hyprland/src/desktop/DesktopTypes.hpp>
#include <hyprland/src/desktop/view/window/Window.hpp>
#include <hyprland/src/render/Context.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/state/WorkspaceState.hpp>
#include <hyprutils/math/Box.hpp>
#include <hyprutils/string/Numeric.hpp>
#include <hyprutils/string/String.hpp>

using namespace Render;

using WORKSPACEID = int64_t;
#ifndef WORKSPACE_INVALID
#define WORKSPACE_INVALID -1L
#endif

typedef void (*render_workspace_t)(
    void* thisptr,
    Render::CRenderContext& ctx,
    PHLMONITOR pMonitor,
    PHLWORKSPACE pWorkspace,
    const Time::steady_tp& now,
    const CBox& geometry
);

typedef bool (*should_render_window_t)(void* thisptr, PHLWINDOW pWindow, PHLMONITOR pMonitor);
typedef void (*render_window_t)(
    void* thisptr,
    Render::CRenderContext& ctx,
    PHLWINDOW pWindow,
    PHLMONITOR pMonitor,
    const Render::SWindowRenderPresentation& presentation,
    const Time::steady_tp& time,
    bool decorate,
    eRenderPassMode mode,
    bool ignorePosition,
    bool overridePosition
);

typedef long VIEWID;

inline WORKSPACEID workspace_to_id(const PHLWORKSPACE& ws) {
    if (!ws)
        return WORKSPACE_INVALID;
    if (auto num = ws->numberedID())
        return static_cast<WORKSPACEID>(*num);
    return WORKSPACE_INVALID;
}

inline WORKSPACEID workspace_to_id(const PHLWORKSPACEREF& ws) {
    auto sp = ws.lock();
    return sp ? workspace_to_id(sp) : WORKSPACE_INVALID;
}

inline WORKSPACEID window_to_workspace_id(const PHLWINDOW& window) {
    if (!window || !window->m_workspace)
        return WORKSPACE_INVALID;
    return workspace_to_id(window->m_workspace);
}

inline PHLWORKSPACE get_workspace_by_id(WORKSPACEID id) {
    if (id == WORKSPACE_INVALID)
        return nullptr;
    if (id > 0)
        return State::workspaceState()->query().numbered(Workspace::SWorkspaceNumberedID{static_cast<Workspace::WorkspaceIDContainer>(id)}).run();
    for (const auto& ws : State::workspaceState()->workspaces()) {
        if (ws && workspace_to_id(ws) == id)
            return ws.lock();
    }
    return nullptr;
}

inline PHLWORKSPACE create_workspace_by_id(WORKSPACEID id, PHLMONITOR monitor) {
    if (id <= 0 || !monitor)
        return nullptr;
    return State::workspaceState()->createNumbered(Workspace::SWorkspaceNumberedID{static_cast<Workspace::WorkspaceIDContainer>(id)}, monitor);
}

inline WORKSPACEID parse_rule_id(const SP<Config::CWorkspaceRule>& rule) {
    if (!rule)
        return WORKSPACE_INVALID;
    if (Hyprutils::String::isNumber(rule->m_workspaceString)) {
        const auto id = Hyprutils::String::strToNumber<WORKSPACEID>(rule->m_workspaceString);
        return id.value_or(WORKSPACE_INVALID);
    }
    return WORKSPACE_INVALID;
}
