#include <wode-wm/keyboard.h>
#include <wode-wm/compositor.h>

namespace wode
{

Keyboard::Keyboard(Compositor &c, wlr_input_device *device) : CompositorComponent(c) {
    keyboard = wlr_keyboard_from_input_device(device);

    xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	xkb_keymap *keymap = xkb_keymap_new_from_names(context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS);

	wlr_keyboard_set_keymap(keyboard, keymap);
	xkb_keymap_unref(keymap);
	xkb_context_unref(context);
	wlr_keyboard_set_repeat_info(keyboard, 25, 600);

    addWaylandSignal(&keyboard->events.modifiers, onKeyModifiers);
    addWaylandSignal(&keyboard->events.key, onKey);
    addWaylandSignal(&device->events.destroy, onKeyDestroy);

    wlr_seat_set_keyboard(compositor.getSeat(), keyboard);
}


bool Keyboard::handleKeyBinding(xkb_keysym_t sym) {

	switch (sym) {
	case XKB_KEY_Escape:
		wl_display_terminate(compositor.getDisplay());
		break;
	case XKB_KEY_F1:
		printf("KEY F1\n");

		setenv("WAYLAND_DISPLAY", compositor.getSocket(), true);
		if (fork() == 0) {
			execl("/bin/sh", "/bin/sh", "-c", "konsole", (void *)NULL);
		}break;
	case XKB_KEY_F2:
		printf("KEY F2\n");

		setenv("WAYLAND_DISPLAY", compositor.getSocket(), true);
		if (fork() == 0) {
			execl("/bin/sh", "/bin/sh", "-c", "firefox", (void *)NULL);
		}break;
	case XKB_KEY_F3:
		printf("KEY F3\n");

		setenv("WAYLAND_DISPLAY", compositor.getSocket(), true);
		if (fork() == 0) {
			execl("/bin/sh", "/bin/sh", "-c", "code", (void *)NULL);
		}break;

		
	default:
		return false;
	} 
    
	return true;
}

void Keyboard::onKeyModifiers(DataObject &obj) {
    wlr_seat_set_keyboard(compositor.getSeat(), keyboard);
	wlr_seat_keyboard_notify_modifiers(compositor.getSeat(), &keyboard->modifiers);
}


void Keyboard::onKey(DataObject &obj) {
    wlr_keyboard_key_event *event = obj;

	wlr_seat *seat = compositor.getSeat();

	// Translate libinput keycode -> xkbcommon 
	uint32_t keycode = event->keycode + 8;

	// Get a list of keysyms based on the keymap for this keyboard 
	const xkb_keysym_t *syms;
	int nsyms = xkb_state_key_get_syms(keyboard->xkb_state, keycode, &syms);

	bool handled = false;

	uint32_t modifiers = wlr_keyboard_get_modifiers(keyboard);

	if ((modifiers & WLR_MODIFIER_ALT) && !(modifiers & WLR_MODIFIER_CTRL) && event->state == WL_KEYBOARD_KEY_STATE_PRESSED) {
		// If alt is held down and this button was _pressed_, we attempt to
		// process it as a compositor keybinding. 
		for (int i = 0; i < nsyms; i++) {
			handled = handleKeyBinding(syms[i]);
		}
	}

	if (!handled) {
		// Otherwise, we pass it along to the client.
		wlr_seat_set_keyboard(seat, keyboard);
		wlr_seat_keyboard_notify_key(seat, event->time_msec, event->keycode, event->state);
	}
}


void Keyboard::onKeyDestroy(DataObject &obj) {
    println("ON onKeyDestroy");
    cleanup();
}


}
