#include <wode-wm/compositor.h>
#include <wode-wm/output.h>
#include <wode-wm/xdg-shell.h>
#include <wode-wm/input.h>
#include <wode-wm/keyboard.h>
#include <wode-wm/popup-window.h>
#include <wode-wm/layer-window.h>


namespace wode
{

CompositorComponent::CompositorComponent(Compositor &compositor) : compositor(compositor) {

}

Compositor::Compositor() {

}

Compositor::~Compositor() = default;

bool Compositor::init() {
    display = wl_display_create();
    backend = wlr_backend_autocreate(wl_display_get_event_loop(display), NULL);
    
    if (backend == NULL) {
		wlr_log(WLR_ERROR, "failed to create wlr_backend");
		return false;
	}

    renderer = wlr_renderer_autocreate(backend);
	if (renderer == NULL) {
		wlr_log(WLR_ERROR, "failed to create wlr_renderer");
		return false;
	}

	wlr_renderer_init_wl_display(renderer, display);

    allocator = wlr_allocator_autocreate(backend, renderer);

	if (allocator == NULL) {
		wlr_log(WLR_ERROR, "failed to create wlr_allocator");
		return false;
	}

    wlr_compositor_create(display, 5, renderer);
	wlr_subcompositor_create(display);
	wlr_data_device_manager_create(display);

    outputLayout = wlr_output_layout_create(display);
    
	scene = wlr_scene_create();
	sceneLayout = wlr_scene_attach_output_layout(scene, outputLayout);

    input = make_unique<Input>(*this, backend);

    addWaylandSignal(&backend->events.new_output, onNewOutput);

    xdgShell = make_unique<XdgShell>(*this, wlr_xdg_shell_create(display, 3));


    seat = wlr_seat_create(display, "seat0");

    addWaylandSignal(&seat->events.request_set_cursor, onRequestCursor);

    println("Set Seat {} to {}", (void*)this, (void*)seat);

    socket = wl_display_add_socket_auto(display);
	if (!socket) {
		wlr_backend_destroy(backend);
		return false;
	}

	if (!wlr_backend_start(backend)) {
		wlr_backend_destroy(backend);
		wl_display_destroy(display);
		return false;
	}

	wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s",
			socket);
    

	setenv("WAYLAND_DISPLAY", getSocket(), true);
	if (fork() == 0) {
        sleep(2);
		execl("/bin/sh", "/bin/sh", "-c", "konsole", (void *)NULL);
	}

    wl_display_run(display);

    return true;
}

void Compositor::onNewOutput(DataObject &data) {
    wlr_output *output = (wlr_output *)data.getUserData();

    wlr_output_init_render(output, allocator, renderer);

    outputs.push_back(make_unique<Output>(*this, output));

    wlr_output_layout_output *l_output = wlr_output_layout_add_auto(outputLayout, output);
	wlr_scene_output *scene_output = wlr_scene_output_create(scene, output);
	wlr_scene_output_layout_add_output(sceneLayout, l_output, scene_output);
}

Output &Compositor::getDefaultOutput() {
    return *outputs[0].get();
}

wlr_surface *Compositor::getSurfaceAt(double lx, double ly, double *sx, double *sy) {
	wlr_scene_node *node = wlr_scene_node_at(&scene->tree.node, lx, ly, sx, sy);

	if (node == NULL || node->type != WLR_SCENE_NODE_BUFFER) {
		return NULL;
	}

	struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
	struct wlr_scene_surface *scene_surface =
		wlr_scene_surface_try_from_buffer(scene_buffer);
	if (!scene_surface) {
		return NULL;
	}

	return scene_surface->surface;
}

void Compositor::onRequestCursor(DataObject &data) {
    wlr_seat_pointer_request_set_cursor_event *event = data;

	struct wlr_seat_client *focused_client = getSeat()->pointer_state.focused_client;

	if (focused_client == event->seat_client) {
		wlr_cursor_set_surface(input->getCursor(), event->surface,
				event->hotspot_x, event->hotspot_y);
	}
}

}
