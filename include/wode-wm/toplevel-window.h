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

    void onRequestMove(DataObject &data);

    void onRequestResize(DataObject &data);

    void onRequestMaximize(DataObject &data);

    void onRequestFullScreen(DataObject &data);

    void beginInteractive(CursorMode mode, uint32_t edges);
    void activate();

    wlr_scene_tree *getSceneTree() { return sceneTree; }
    wlr_xdg_toplevel *getTopLevel() { return toplevel; }
private:
    XdgShell &shell;
    wlr_xdg_toplevel *toplevel;
    wlr_scene_tree *sceneTree;
};

}
