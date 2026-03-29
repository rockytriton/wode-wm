#include <wode-wm/layer-window.h>
#include <wode-wm/compositor.h>
#include <wode-wm/output.h>
#include <wode-wm/xdg-shell.h>
#include <wode-wm/toplevel-window.h>
#include <wode-wm/popup-window.h>

namespace wode
{

LayerWindow::LayerWindow(Compositor &c, XdgShell &shell, wlr_layer_surface_v1 *layerSurface) : Window(c, layerSurface->surface), shell(shell) {
    this->layerSurface = layerSurface;

    wlr_scene_tree *parent_tree = (wlr_scene_tree *)&compositor.getScene()->tree;

    sceneLayerSurface = wlr_scene_layer_surface_v1_create(parent_tree, layerSurface);
    surface->data = sceneLayerSurface->tree;

    addWaylandSignal(&layerSurface->events.new_popup, onPopup);
}

void LayerWindow::onPopup(DataObject &data) {
    println("ON LayerWindow onPopup");
    wlr_xdg_popup *popup = data;

    wlr_scene_tree *my_tree = (wlr_scene_tree *)surface->data;

    new PopupWindow(compositor, shell, popup, my_tree);
}

void LayerWindow::onCommit(DataObject &data) {
    if (layerSurface->initial_commit) {
        // Now it's safe to configure!
        wlr_layer_surface_v1_configure(layerSurface, 
                                       layerSurface->pending.desired_width, 
                                       layerSurface->pending.desired_height);

        shell.arrangeLayers();
    }

    bool changed = (layerSurface->current.anchor != layerSurface->pending.anchor) ||
                   (layerSurface->current.exclusive_zone != layerSurface->pending.exclusive_zone) ||
                   (layerSurface->current.actual_width != layerSurface->pending.actual_width) ||
                   (layerSurface->current.actual_height != layerSurface->pending.actual_height);

    if (changed) {
        printf("State changed, rearranging...\n");
        shell.arrangeLayers();
    }
}

void LayerWindow::onMap(DataObject &data) {
    
}

void LayerWindow::onUnmap(DataObject &data) {
    cleanup();
}

}
