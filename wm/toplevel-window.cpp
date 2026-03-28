#include <wode-wm/toplevel-window.h>
#include <wode-wm/compositor.h>
#include <wode-wm/output.h>
#include <wode-wm/xdg-shell.h>

namespace wode {

TopLevelWindow::TopLevelWindow(Compositor &c, XdgShell &shell, wlr_xdg_toplevel *tl) : Window(c, tl->base->surface), shell(shell), toplevel(tl) {
    sceneTree = wlr_scene_xdg_surface_create(&compositor.getScene()->tree, toplevel->base);
    sceneTree->node.data = this;
    toplevel->base->data = sceneTree;

    addWaylandSignal(&toplevel->base->surface->events.commit, onCommit);
    addWaylandSignal(&toplevel->base->surface->events.map, onMap);
    addWaylandSignal(&toplevel->base->surface->events.unmap, onUnmap);
}


void TopLevelWindow::onCommit(DataObject &data) {
    println("ON TopLevelWindow COMMIT: {}", toplevel->base->initialized);

    if (toplevel->base->initial_commit) {
        println("SET SIZE");
        wlr_xdg_toplevel_set_size(toplevel, 0, 0);
    }

}

void TopLevelWindow::onMap(DataObject &data) {
    println("ON TopLevelWindow MAP");

    Dimensions dimensions = compositor.getDefaultOutput().getDimensions();

	// In plxwm_appwindow.cpp
	if (toplevel->pending.width < 600 && toplevel->pending.height < 400) {
		// Center the window on the output
		
		printf("%d, %d\n", dimensions.width, dimensions.height);
		
		int center_x = (dimensions.width / 2);
		int center_y = (dimensions.height / 2);

		wlr_scene_node_set_position(&sceneTree->node, 10, 10);
	}

    shell.focus(*this);
}

void TopLevelWindow::onUnmap(DataObject &data) {
    println("ON TopLevelWindow UNMAP");
}

void TopLevelWindow::activate() {
    wlr_scene_node_raise_to_top(&sceneTree->node);

	wlr_xdg_toplevel_set_activated(toplevel, true);
}

}
