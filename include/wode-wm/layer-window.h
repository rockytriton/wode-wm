#pragma once

#include <wode-wm/window.h>

namespace wode
{

class Compositor;
class XdgShell;

class LayerWindow : public Window {
public:
    LayerWindow(Compositor &c, XdgShell &shell, wlr_layer_surface_v1 *sceneLayerSurface);

    virtual void onCommit(DataObject &data);
    virtual void onMap(DataObject &data);
    virtual void onUnmap(DataObject &data);

    void onPopup(DataObject &data);

    wlr_scene_layer_surface_v1 *getSceneSurface() { return sceneLayerSurface; }
    wlr_layer_surface_v1 *getLayerSurface() { return layerSurface; }

private:
    XdgShell &shell;
    wlr_scene_layer_surface_v1 *sceneLayerSurface;
    wlr_layer_surface_v1 *layerSurface;
    
};

}
