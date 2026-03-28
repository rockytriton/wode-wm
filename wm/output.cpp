#include <wode-wm/output.h>
#include <wode-wm/compositor.h>

namespace wode
{

Output::Output(Compositor &c, wlr_output *output) : compositor(c), output(output) {
    wlr_output_state state;
	wlr_output_state_init(&state);
	wlr_output_state_set_scale(&state, 1.5);
	wlr_output_state_set_enabled(&state, true);

    wlr_output_mode *mode = wlr_output_preferred_mode(output);

    if (mode) {
        wlr_output_state_set_mode(&state, mode);
    }

    wlr_output_commit_state(output, &state);
	wlr_output_state_finish(&state);

    addWaylandSignal(&output->events.request_state, onRequestState);
    addWaylandSignal(&output->events.frame, onFrame);
    addWaylandSignal(&output->events.destroy, onDestroy);
}

Dimensions Output::getDimensions() {
    int output_width = output->width;
	int output_height = output->height;
	
	wlr_output_effective_resolution(output, &output_width, &output_height);

    return Dimensions {
        .width = output_width,
        .height = output_height
    };
}

void Output::onFrame(DataObject &data) {
    //printf("onFrame\n");

	/* This function is called every time an output is ready to display a frame,
	 * generally at the output's refresh rate (e.g. 60Hz). */
	wlr_scene *scene = compositor.getScene();

	struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(
		scene, this->output);

	// Render the scene if needed and commit the output 
	wlr_scene_output_commit(scene_output, NULL);

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	wlr_scene_output_send_frame_done(scene_output, &now); 
}

void Output::onRequestState(DataObject &data) {
	const wlr_output_event_request_state *event = (wlr_output_event_request_state *)data.getUserData();
	wlr_output_commit_state(output, event->state);
}

void Output::onDestroy(DataObject &data) {
	cleanup();
}


}
