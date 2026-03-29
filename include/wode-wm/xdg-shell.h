#pragma once

#include <wode-wm/wayland.h>
#include <wode-wm/toplevel-window.h>

namespace wode
{

class Compositor;
class TopLevelWindow;
class PopupWindow;
class LayerWindow;

class XdgShell : public CompositorComponent {
public:
    XdgShell(Compositor &c, wlr_xdg_shell *shell);

    void newTopLevel(DataObject &data);
    void newPopup(DataObject &data);
    void onNewLayerSurface(DataObject &data);

    void focus(TopLevelWindow &window);
    
    TopLevelWindow *getGrabbedWindow() { return grabbedWindow; }

    void setGrabbedWindow(TopLevelWindow *w) { grabbedWindow = w; }

    int getGrabX() { return grabX; }
    int getGrabY() { return grabY; }

    void setGrabX(int x) { grabX = x; }
    void setGrabY(int y) { grabY = y; }

	wlr_box getGrabGeoBox() { return grab_geobox; }
	void setGrabGeoBox(wlr_box g) { grab_geobox = g; }
	void setResizeEdgets(uint32_t e) { resize_edges = e; }

    TopLevelWindow *getWindowAt(double lx, double ly, double *sx, double *sy);

    void processResize();

    void arrangeLayers();
private:
    TopLevelWindow *grabbedWindow;

    vector<unique_ptr<TopLevelWindow>> toplevels;
    vector<unique_ptr<PopupWindow>> popups;

    vector<unique_ptr<LayerWindow>> layers;

	wlr_xdg_shell *shell;

	int grabX;
    int grabY;
	wlr_box grab_geobox;

	uint32_t resize_edges;
};

}
