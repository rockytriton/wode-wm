#pragma once

#include <wode-wm/wode.h>
#include <wode-wm/wayland.h>

namespace wode
{

class Output;
class XdgShell;
class Input;

class Compositor : public Component {
public:
    Compositor();
    ~Compositor();

    bool init();

    void onNewOutput(DataObject &data);
    void onNewInput(DataObject &data);

    void onRequestCursor(DataObject &data);

    void onOutputManagerApply(DataObject &data);
    void onOutputManagerTest(DataObject &data);

	const char *getSocket() { return socket; }

    wlr_scene *getScene() { return scene; }

    Output &getDefaultOutput();
    wlr_seat *getSeat() { return seat; }

    wl_display *getDisplay() { return display; }

    wlr_output_layout *getOutputLayout() { return outputLayout; }

    XdgShell *getXdgShell() { return xdgShell.get(); }

    wlr_surface *getSurfaceAt(double lx, double ly, double *sx, double *sy);

    Input *getInput() { return input.get(); }

    void spawn(const std::string &command);

private:

	wl_display *display;
	wlr_backend *backend;
	wlr_renderer *renderer;
	wlr_allocator *allocator;
	wlr_scene *scene;

	wlr_scene_output_layout *sceneLayout;
    wlr_output_layout *outputLayout;
    wlr_output_manager_v1 *outputManager;

    wlr_seat *seat;
    const char *socket;

    unique_ptr<XdgShell> xdgShell;
    vector<unique_ptr<Output>> outputs;
    unique_ptr<Input> input;
};

}
