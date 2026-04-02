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
	outputManager = wlr_output_manager_v1_create(display);

	addWaylandSignal(&outputManager->events.apply, onOutputManagerApply);
	addWaylandSignal(&outputManager->events.test, onOutputManagerTest);

    
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

	spawn("/usr/bin/wode-shell-panel");
	//spawn("plxwm-panel");

	/*
	if (fork() == 0) {
        sleep(3);
		execl("/bin/sh", "/bin/sh", "-c", "konsole", (void *)NULL);
	}
	*/

	/*

	if (fork() == 0) {
        //sleep(5);
		//execl("/bin/sh", "/bin/sh", "-c", "wode-shell-panel", (void *)NULL);
	}

	if (fork() == 0) {
		//execl("/bin/sh", "/bin/sh", "-c", "/data2/git/plxwm/plxwm-panel", (void *)NULL);
	}
	*/
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

	// 1. Create the configuration
	struct wlr_output_configuration_v1 *config = wlr_output_configuration_v1_create();

	struct wlr_output_layout_output *layout_output;
	wl_list_for_each(layout_output, &outputLayout->outputs, link) {
		// 2. Create the head
		struct wlr_output_configuration_head_v1 *config_head = 
			wlr_output_configuration_head_v1_create(config, layout_output->output);
		
		// 3. Instead of state_init, we manually enable it and sync the mode
		config_head->state.enabled = layout_output->output->enabled;
		config_head->state.mode = layout_output->output->current_mode;
		
		// If you are using custom modes (non-standard resolutions)
		config_head->state.custom_mode.width = layout_output->output->width;
		config_head->state.custom_mode.height = layout_output->output->height;
		config_head->state.custom_mode.refresh = layout_output->output->refresh;

		// Sync position
		config_head->state.x = layout_output->x;
		config_head->state.y = layout_output->y;
	}

	// 4. Push to the manager
	wlr_output_manager_v1_set_configuration(this->outputManager, config);
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

void Compositor::onOutputManagerApply(DataObject &data) {
	println("ON onOutputManagerApply");

	struct wlr_output_configuration_v1 *config = (struct wlr_output_configuration_v1 *)data;
    struct wlr_output_configuration_head_v1 *config_head;

	int len = wl_list_length(&config->heads);

	FILE *fp = fopen("/tmp/mode.log", "wb");

	fprintf(fp, "onOutputManagerApply: LEN: %d\n", len); fflush(fp);

    wl_list_for_each(config_head, &config->heads, link) {
	fprintf(fp, "FOR EACH: %p\n", config_head);
	fflush(fp);
        struct wlr_output *output = config_head->state.output;

	fprintf(fp, "OUTPUT: %p\n", output); fflush(fp);

        struct wlr_output_state state;
        
	fprintf(fp, "INITING\n"); fflush(fp);
	wlr_output_state_init(&state);


		fprintf(fp, "Applying state mode: %p - %p\n", config_head, config_head->state.mode); fflush(fp);

		if (!config_head->state.mode) {
			fprintf(fp, "NOPE NOT SETTING\n"); fflush(fp);
			return;
		}

        // Apply the requested mode from the panel
        if (config_head->state.mode) {
			fprintf(fp, "Applying state mode: %d X %d\n", config_head->state.mode->width, config_head->state.mode->height); fflush(fp);
            wlr_output_state_set_mode(&state, config_head->state.mode);
        } else {
            // Handle custom modes if the panel sent raw W/H
			fprintf(fp, "Applying custom mode: %d X %d\n", config_head->state.custom_mode.width, config_head->state.custom_mode.height); fflush(fp);
            wlr_output_state_set_custom_mode(&state, 
                config_head->state.custom_mode.width,
                config_head->state.custom_mode.height,
                config_head->state.custom_mode.refresh);
        }


	fprintf(fp, "Committing\n");fflush(fp);
        wlr_output_commit_state(output, &state);
        wlr_output_state_finish(&state);
    }
    
      fprintf(fp, "DONE\n");

	fflush(fp);
	fclose(fp);

    // Tell the manager the change was successful
    wlr_output_configuration_v1_send_succeeded(config);
    wlr_output_configuration_v1_destroy(config);
}

void Compositor::onOutputManagerTest(DataObject &data) {
	println("ON onOutputManagerTest");
}


void Compositor::spawn(const std::string &command) {
	pid_t pid = fork();

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command.data(), (char *)NULL);
        println(stderr, "Failed to spawn process: {}", command);
        _exit(1);
    } else if (pid < 0) {
        println(stderr, "Fork failed for command: {}", command);
    }
}

}
