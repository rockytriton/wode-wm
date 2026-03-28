#include <wode-wm/xdg-shell.h>
#include <wode-wm/toplevel-window.h>
#include <wode-wm/compositor.h>

namespace wode
{

XdgShell::XdgShell(Compositor &c, wlr_xdg_shell *shell) : CompositorComponent(c), shell(shell) {
    addWaylandSignal(&shell->events.new_toplevel, newTopLevel);
    addWaylandSignal(&shell->events.new_popup, newPopup);
    
}

void XdgShell::newTopLevel(DataObject &data) {
    println("ON XDG TOP LEVEL: {}", data.getUserData());
    toplevels.push_back(make_unique<TopLevelWindow>(compositor, *this, (wlr_xdg_toplevel *)data.getUserData()));
}

void XdgShell::newPopup(DataObject &data) {

}

void XdgShell::focus(TopLevelWindow &window) {

	wlr_surface *prev_surface = compositor.getSeat()->keyboard_state.focused_surface;
	wlr_surface *surface = window.getSurface();

	if (prev_surface == surface) {
        println("Prev Surface");
		return;
	}

	if (prev_surface) {
		wlr_xdg_toplevel *prev_toplevel = wlr_xdg_toplevel_try_from_wlr_surface(prev_surface);

		if (prev_toplevel != NULL) {
			wlr_xdg_toplevel_set_activated(prev_toplevel, false);
		}
	}

	wlr_keyboard *keyboard = wlr_seat_get_keyboard(compositor.getSeat());

    println("Keyboard: {}", (void *)keyboard);

	window.activate();

	if (keyboard != NULL) {
		wlr_seat_keyboard_notify_enter(compositor.getSeat(), surface,
			keyboard->keycodes, keyboard->num_keycodes, &keyboard->modifiers);
	}

    println("Focused");
}
    

}

