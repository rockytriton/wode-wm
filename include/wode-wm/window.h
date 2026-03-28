#pragma once

#include <wode-wm/wayland.h>

namespace wode
{

class Compositor;

class Window : public Component {
public:
    Window(Compositor &c, wlr_surface *surface);

    virtual void onCommit(DataObject &data) {}
    virtual void onMap(DataObject &data) {}
    virtual void onUnmap(DataObject &data) {}

    virtual wlr_surface *getSurface() { return surface; }

protected:
    Compositor &compositor;
    wlr_surface *surface;
};

}
