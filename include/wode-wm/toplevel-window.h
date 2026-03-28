#pragma once

#include <wode-wm/window.h>

namespace wode
{

class Compositor;
class XdgShell;

class TopLevelWindow : public Window {
public:
    TopLevelWindow(Compositor &c, XdgShell &shell, wlr_xdg_toplevel *tl);


    virtual void onCommit(DataObject &data);
    virtual void onMap(DataObject &data);
    virtual void onUnmap(DataObject &data);

    void activate();

    virtual wlr_surface *getSurface() { return toplevel->base->surface; }

private:
    XdgShell &shell;
    wlr_xdg_toplevel *toplevel;
    wlr_scene_tree *sceneTree;
};

}
