#pragma once

#include <wode-wm/wayland.h>

namespace wode
{

class Compositor;

class Keyboard : public CompositorComponent {
public:
    Keyboard(Compositor &c, wlr_input_device *device);

    void onKeyModifiers(DataObject &obj);
    void onKey(DataObject &obj);
    void onKeyDestroy(DataObject &obj);

private:

    bool handleKeyBinding(xkb_keysym_t sym);

    wlr_keyboard *keyboard;
};

}
