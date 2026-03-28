#pragma once

#include <wode-wm/wayland.h>

namespace wode
{

class Compositor;

class Output : public Component {
public:
    Output(Compositor &c, wlr_output *out);

    wlr_output *getWaylandOutput() { return output; }

    Dimensions getDimensions();

    void onFrame(DataObject &data);
    void onRequestState(DataObject &data);
    void onDestroy(DataObject &data);

private:
    Compositor &compositor;
	wlr_output *output;
};

}
