#include <wode-wm/popup-window.h>
#include <wode-wm/compositor.h>
#include <wode-wm/output.h>
#include <wode-wm/xdg-shell.h>
#include <wode-wm/toplevel-window.h>

namespace wode
{

PopupWindow::PopupWindow(Compositor &c, XdgShell &shell, wlr_xdg_popup *popup, wlr_scene_tree *parentTree) : Window(c, popup->base->surface), 
                                                                                                             shell(shell), popup(popup) {

    if (!parentTree) {
        if (popup->parent == nullptr) {
            parentTree = (wlr_scene_tree *)&compositor.getScene()->tree;
        } else {
            wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(popup->parent);

            if (parent != nullptr) {
                parentTree = (wlr_scene_tree *)parent->data;
            } else {
                println("WARN: PARENT TREE NULL");
            }
        }
        
    }

    sceneTree = wlr_scene_xdg_surface_create(parentTree, popup->base);
    popup->base->data = sceneTree;

}

void PopupWindow::onCommit(DataObject &data) {
    //println("POPUP COMMIT: {} - {}", (void*)popup, popup->base->initial_commit);

    if (popup->base->initial_commit) {
		wlr_xdg_surface_schedule_configure(popup->base);
	}

    if (popup->base->surface->mapped) {
        wlr_scene_node *node = &((wlr_scene_tree *)popup->base->data)->node;

        auto dimensions = compositor.getDefaultOutput().getDimensions();

        int lx, ly;
        wlr_xdg_popup_get_toplevel_coords(popup, 
                                        popup->current.geometry.x, 
                                        popup->current.geometry.y, 
                                        &lx, &ly);

        int x = popup->pending.geometry.x;
        int y = popup->pending.geometry.y;

        int w = popup->pending.geometry.width;
        int h = popup->pending.geometry.height;

        int diffY = dimensions.height - (ly + h);
        int diffX = dimensions.width - (lx + w);

        if (diffY < 0) {
            y += diffY;
        }
        if (diffX < 0) {
            x += diffX;
        }

        if (popup->parent != nullptr && 
            popup->parent->role != nullptr && 
            strcmp(popup->parent->role->name, "zwlr_layer_surface_v1") == 0) {
            
            wlr_scene_node_set_position(node, x, y);
        } else {
            wlr_scene_node_set_position(node, x, y);
        }
                                        
        wlr_scene_node_set_enabled(node, true);
    }
}

void PopupWindow::onMap(DataObject &data) {

}

void PopupWindow::onUnmap(DataObject &data) {
    println("POPUP UNMAP");
    cleanup();
}


}