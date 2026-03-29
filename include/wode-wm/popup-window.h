#pragma once

#include <wode-wm/window.h>

namespace wode
{

class Compositor;
class XdgShell;

class PopupWindow : public Window {
public:
    PopupWindow(Compositor &c, XdgShell &shell, wlr_xdg_popup *popup, wlr_scene_tree *parentTree);

    virtual void onCommit(DataObject &data);
    virtual void onMap(DataObject &data);
    virtual void onUnmap(DataObject &data);

    wlr_scene_tree *getSceneTree() { return sceneTree; }
    wlr_xdg_popup *getPopup() { return popup; }
private:
    XdgShell &shell;
	wlr_xdg_popup *popup;
    wlr_scene_tree *sceneTree;
    
};

}
