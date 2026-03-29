#include <wode-wm/toplevel-window.h>
#include <wode-wm/compositor.h>
#include <wode-wm/output.h>
#include <wode-wm/input.h>
#include <wode-wm/xdg-shell.h>

namespace wode {

TopLevelWindow::TopLevelWindow(Compositor &c, XdgShell &shell, wlr_xdg_toplevel *tl) : Window(c, tl->base->surface), shell(shell), toplevel(tl) {
    sceneTree = wlr_scene_xdg_surface_create(&compositor.getScene()->tree, toplevel->base);
    sceneTree->node.data = this;
    toplevel->base->data = sceneTree;

    addWaylandSignal(&toplevel->events.request_move, onRequestMove);
    addWaylandSignal(&toplevel->events.request_resize, onRequestResize);
    addWaylandSignal(&toplevel->events.request_maximize, onRequestMaximize);
    addWaylandSignal(&toplevel->events.request_fullscreen, onRequestFullScreen);
}

void TopLevelWindow::onCommit(DataObject &data) {
    if (toplevel->base->initial_commit) {
        wlr_xdg_toplevel_set_size(toplevel, 0, 0);
    }
}

void TopLevelWindow::onMap(DataObject &data) {
    Dimensions dimensions = compositor.getDefaultOutput().getDimensions();

	int width = getSurface()->current.width;
    int height = getSurface()->current.height;

    printf("%d, %d\n", dimensions.width, dimensions.height);

    int center_x = (dimensions.width / 2) - (width / 2);
    int center_y = (dimensions.height / 2) - (height / 2);

    wlr_scene_node_set_position(&sceneTree->node, center_x, center_y);
	
    shell.focus(*this);
}

void TopLevelWindow::onUnmap(DataObject &data) {
    println("ON TopLevelWindow UNMAP");
}

void TopLevelWindow::beginInteractive(CursorMode mode, uint32_t edges) {
	compositor.getXdgShell()->setGrabbedWindow(this);
	compositor.getInput()->setCursorMode(mode);

	if (mode == TINYWL_CURSOR_MOVE) {
		compositor.getXdgShell()->setGrabX(compositor.getInput()->getCursor()->x - getSceneTree()->node.x);
		compositor.getXdgShell()->setGrabY(compositor.getInput()->getCursor()->y - getSceneTree()->node.y);
	} else {
		struct wlr_box *geo_box = &toplevel->base->geometry;

		double border_x = (getSceneTree()->node.x + geo_box->x) +
			((edges & WLR_EDGE_RIGHT) ? geo_box->width : 0);
		double border_y = (getSceneTree()->node.y + geo_box->y) +
			((edges & WLR_EDGE_BOTTOM) ? geo_box->height : 0);
		
		compositor.getXdgShell()->setGrabX(compositor.getInput()->getCursor()->x - border_x);
		compositor.getXdgShell()->setGrabY(compositor.getInput()->getCursor()->y - border_y);

		wlr_box gb = *geo_box;
		gb.x += getSceneTree()->node.x;
		gb.y += getSceneTree()->node.y;
		compositor.getXdgShell()->setGrabGeoBox(gb);

		compositor.getXdgShell()->setResizeEdgets(edges);
	}
}

void TopLevelWindow::onRequestMove(DataObject &data) {
	printf("ON TopLevelWindow::onRequestMove\n");
	beginInteractive(TINYWL_CURSOR_MOVE, 0);
}

void TopLevelWindow::onRequestResize(DataObject &data) {
	printf("ON TopLevelWindow::onRequestMove\n");
    wlr_xdg_toplevel_resize_event *event = data;
	beginInteractive(TINYWL_CURSOR_RESIZE, event->edges);
}

void TopLevelWindow::onRequestMaximize(DataObject &data) {
	//not yet supported...

	if (toplevel->base->initialized) {
		wlr_xdg_surface_schedule_configure(toplevel->base);
	}
}

void TopLevelWindow::onRequestFullScreen(DataObject &data) {
    //not yet supported...

	if (toplevel->base->initialized) {
		wlr_xdg_surface_schedule_configure(toplevel->base);
	}
}

void TopLevelWindow::activate() {
    wlr_scene_node_raise_to_top(&sceneTree->node);

	wlr_xdg_toplevel_set_activated(toplevel, true);
}

}
