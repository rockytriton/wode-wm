#pragma once

#include <wode-wm/wayland.h>

namespace wode
{

class Compositor;
class Keyboard;

class Input : public CompositorComponent {
public:
    Input(Compositor &c, wlr_backend *backend);

    void onNewInput(DataObject &data);

    void onCursorMove(uint32_t time);
    void onCursorMotion(DataObject &obj);
    void onCursorMotionAbsolute(DataObject &obj);
    void onCursorButton(DataObject &obj);
    void onCursorFrame(DataObject &obj);
    void onCursorAxis(DataObject &obj);

    void setCursorMode(CursorMode mode) { cursor_mode = mode; }

    wlr_cursor *getCursor() { return cursor; }
private:

    bool handleKeyBinding(xkb_keysym_t sym);

    vector<unique_ptr<Keyboard>> keyboards;

    wlr_cursor *cursor;
	wlr_xcursor_manager *cursor_mgr;
    CursorMode cursor_mode;
};

}
