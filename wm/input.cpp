#include <wode-wm/input.h>
#include <wode-wm/keyboard.h>
#include <wode-wm/compositor.h>
#include <wode-wm/xdg-shell.h>
#include <wode-wm/toplevel-window.h>

namespace wode
{

Input::Input(Compositor &c, wlr_backend *backend) : CompositorComponent(c) {
    addWaylandSignal(&backend->events.new_input, onNewInput);

	cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(cursor, c.getOutputLayout());

	cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	cursor_mode = TINYWL_CURSOR_PASSTHROUGH;

    addWaylandSignal(&cursor->events.motion, onCursorMotion);
    addWaylandSignal(&cursor->events.motion_absolute, onCursorMotionAbsolute);
    addWaylandSignal(&cursor->events.button, onCursorButton);
    addWaylandSignal(&cursor->events.axis, onCursorAxis);
    addWaylandSignal(&cursor->events.frame, onCursorFrame);
}

void Input::onNewInput(DataObject &data) {
    wlr_input_device *device = data;

    switch (device->type) {
	case WLR_INPUT_DEVICE_KEYBOARD:
        keyboards.push_back(make_unique<Keyboard>(compositor, device));
		break;
	case WLR_INPUT_DEVICE_POINTER:
        wlr_cursor_attach_input_device(cursor, device);
		break;
	default:
		break;
	}

	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;

	if (!keyboards.empty()) {
		caps |= WL_SEAT_CAPABILITY_KEYBOARD;
	}

	wlr_seat_set_capabilities(compositor.getSeat(), caps);
}

void Input::onCursorMove(uint32_t time) {
    if (cursor_mode == TINYWL_CURSOR_MOVE && compositor.getXdgShell()->getGrabbedWindow()) {
		
        TopLevelWindow *wnd = compositor.getXdgShell()->getGrabbedWindow();

        wlr_scene_node_set_position(&wnd->getSceneTree()->node,
            cursor->x - compositor.getXdgShell()->getGrabX(),
            cursor->y - compositor.getXdgShell()->getGrabY());

		return;
	} else if (cursor_mode == TINYWL_CURSOR_RESIZE && compositor.getXdgShell()->getGrabbedWindow()) {
        compositor.getXdgShell()->processResize();
		return;
	}

	double sx, sy;
	struct wlr_seat *seat = compositor.getSeat();
	struct wlr_surface *surface = compositor.getSurfaceAt(cursor->x, cursor->y, &sx, &sy);
	
	if (surface) {
		wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
		wlr_seat_pointer_notify_motion(seat, time, sx, sy);
	} else {
		wlr_cursor_set_xcursor(cursor, cursor_mgr, "default");
		wlr_seat_pointer_clear_focus(seat);
	}
}


void Input::onCursorMotion(DataObject &obj) {
    wlr_pointer_motion_event *event = obj;
    wlr_cursor_move(cursor, &event->pointer->base,
			event->delta_x, event->delta_y);

    onCursorMove(event->time_msec);
}


void Input::onCursorMotionAbsolute(DataObject &obj) {
    wlr_pointer_motion_absolute_event *event = obj;
    wlr_cursor_warp_absolute(cursor, &event->pointer->base, event->x, event->y);

    onCursorMove(event->time_msec);
}


void Input::onCursorButton(DataObject &obj) {
    wlr_pointer_button_event *event = obj;

    wlr_seat_pointer_notify_button(compositor.getSeat(),
			event->time_msec, event->button, event->state);
			
	if (event->state == WL_POINTER_BUTTON_STATE_RELEASED) {
		cursor_mode = TINYWL_CURSOR_PASSTHROUGH;
		compositor.getXdgShell()->setGrabbedWindow(NULL);
	} else {
		double sx, sy;

		TopLevelWindow *wnd = compositor.getXdgShell()->getWindowAt(cursor->x, cursor->y, &sx, &sy);

        if (wnd) {
		    compositor.getXdgShell()->focus(*wnd);
        }
	}
}


void Input::onCursorFrame(DataObject &obj) {
    wlr_seat_pointer_notify_frame(compositor.getSeat());
    
}


void Input::onCursorAxis(DataObject &obj) {
    wlr_pointer_axis_event *event = obj;
    wlr_seat_pointer_notify_axis(compositor.getSeat(),
			event->time_msec, event->orientation, event->delta,
			event->delta_discrete, event->source, event->relative_direction);
}



}
