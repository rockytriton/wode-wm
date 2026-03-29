#include <wode-wm/xdg-shell.h>
#include <wode-wm/popup-window.h>
#include <wode-wm/toplevel-window.h>
#include <wode-wm/layer-window.h>
#include <wode-wm/compositor.h>
#include <wode-wm/input.h>
#include <wode-wm/output.h>

namespace wode
{

XdgShell::XdgShell(Compositor &c, wlr_xdg_shell *shell) : CompositorComponent(c), shell(shell) {
    addWaylandSignal(&shell->events.new_toplevel, newTopLevel);
    addWaylandSignal(&shell->events.new_popup, newPopup);
    
    auto layer_shell = wlr_layer_shell_v1_create(compositor.getDisplay(), 4);

	addWaylandSignal(&layer_shell->events.new_surface, onNewLayerSurface);

}

void XdgShell::newTopLevel(DataObject &data) {
    println("ON XDG TOP LEVEL: {}", data.getUserData());
    toplevels.push_back(make_unique<TopLevelWindow>(compositor, *this, (wlr_xdg_toplevel *)data.getUserData()));
}

void XdgShell::newPopup(DataObject &data) {
    wlr_xdg_popup *event = data;

    if (event->parent == nullptr) {
        println("WARN: NULL PARENT");
        return;
    }

    popups.push_back(make_unique<PopupWindow>(compositor, *this, event, nullptr));
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
 
TopLevelWindow *XdgShell::getWindowAt(double lx, double ly, double *sx, double *sy) {
	wlr_scene_node *node = wlr_scene_node_at(&compositor.getScene()->tree.node, lx, ly, sx, sy);

	if (node == nullptr || node->type != WLR_SCENE_NODE_BUFFER) {
		return nullptr;
	}

	struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
	struct wlr_scene_surface *scene_surface =
		wlr_scene_surface_try_from_buffer(scene_buffer);
		
	if (!scene_surface) {
		return nullptr;
	}

	struct wlr_scene_tree *tree = node->parent;
	
	while (tree != nullptr && tree->node.data == nullptr) {
		tree = tree->node.parent;
	}

	if (tree == nullptr) {
		return nullptr;
	}
	
	return (TopLevelWindow *)tree->node.data;
}

void XdgShell::processResize() {

	TopLevelWindow *wnd = getGrabbedWindow();

	double border_x = compositor.getInput()->getCursor()->x - grabX;
	double border_y = compositor.getInput()->getCursor()->y - grabY;
	int new_left = grab_geobox.x;
	int new_right = grab_geobox.x + grab_geobox.width;
	int new_top = grab_geobox.y;
	int new_bottom = grab_geobox.y + grab_geobox.height;

	if (resize_edges & WLR_EDGE_TOP) {
		new_top = border_y;
		if (new_top >= new_bottom) {
			new_top = new_bottom - 1;
		}
	} else if (resize_edges & WLR_EDGE_BOTTOM) {
		new_bottom = border_y;
		if (new_bottom <= new_top) {
			new_bottom = new_top + 1;
		}
	}
	if (resize_edges & WLR_EDGE_LEFT) {
		new_left = border_x;
		if (new_left >= new_right) {
			new_left = new_right - 1;
		}
	} else if (resize_edges & WLR_EDGE_RIGHT) {
		new_right = border_x;
		if (new_right <= new_left) {
			new_right = new_left + 1;
		}
	}

	struct wlr_box *geo_box = &wnd->getTopLevel()->base->geometry;
	wlr_scene_node_set_position(&wnd->getSceneTree()->node,
		new_left - geo_box->x, new_top - geo_box->y);

	int new_width = new_right - new_left;
	int new_height = new_bottom - new_top;
	wlr_xdg_toplevel_set_size(wnd->getTopLevel(), new_width, new_height);

}

void XdgShell::arrangeLayers() {
    struct wlr_box full_area = {0};

    auto dimensions = compositor.getDefaultOutput().getDimensions();

    full_area.height = dimensions.height;
    full_area.width = dimensions.width;
    
    struct wlr_box usable_area = full_area;

    for (size_t i=0; i<layers.size(); i++) {
        LayerWindow *layer_win = layers[i].get();
        auto *scene_surface = layer_win->getSceneSurface();
        auto *surface = layer_win->getLayerSurface();

        wlr_scene_layer_surface_v1_configure(scene_surface, &full_area, &usable_area);

        wlr_layer_surface_v1_configure(surface, full_area.width, surface->pending.desired_height);

        int panel_height = surface->current.actual_height;

        if (panel_height == 0) panel_height = 40; // fallback if not yet committed

        wlr_scene_node_set_position(&scene_surface->tree->node, 0, full_area.height - panel_height);
    }
}

void XdgShell::onNewLayerSurface(DataObject &data) {
    layers.push_back(make_unique<LayerWindow>(compositor, *this, data));
}

}

