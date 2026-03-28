#pragma once

#include <wode-wm/wode.h>
#include <wode-wm/wayland.h>

namespace wode
{

class Output;
class XdgShell;

class Compositor : public Component {
public:
    Compositor();
    ~Compositor();

    bool init();

    void onNewOutput(DataObject &data);

	const char *getSocket() { return socket; }

    wlr_scene *getScene() { return scene; }

    Output &getDefaultOutput();
    wlr_seat *getSeat() { return seat; }
    
private:

	wl_display *display;
	wlr_backend *backend;
	wlr_renderer *renderer;
	wlr_allocator *allocator;
	wlr_scene *scene;

	wlr_scene_output_layout *sceneLayout;
    wlr_output_layout *outputLayout;

    wlr_seat *seat;
    const char *socket;

    unique_ptr<XdgShell> xdgShell;
    vector<unique_ptr<Output>> outputs;
};

}
