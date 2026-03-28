#pragma once

#include <wode-wm/wayland.h>
#include <wode-wm/toplevel-window.h>

namespace wode
{

class Compositor;
class TopLevelWindow;

class XdgShell : public CompositorComponent {
public:
    XdgShell(Compositor &c, wlr_xdg_shell *shell);

    void newTopLevel(DataObject &data);
    void newPopup(DataObject &data);

    void focus(TopLevelWindow &window);
    
private:
    vector<unique_ptr<TopLevelWindow>> toplevels;
	wlr_xdg_shell *shell;
};

}
