#pragma once

#include "wode.h"

#ifdef __cplusplus
extern "C" {

#define static
//#define namespace _namespace
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>
//#include <wlr/types/wlr_layer_shell_v1.h>
#undef static
//#undef namespace
}
#endif

namespace wode 
{

struct WaylandSignalData {
    wl_listener listener;
    std::function<void(DataObject &data)> func;
};

class WaylandComponentSignal : public ComponentSignal {
public:
    WaylandComponentSignal(wl_signal *signal, std::function<void(DataObject &data)> func) {
        signalData.func = func;

        signalData.listener.notify = [](struct wl_listener *l, void *data) {
            WaylandSignalData *sigData = (WaylandSignalData *)l;
            DataObject obj(data);
            sigData->func(obj);
        };

        wl_signal_add(signal, &signalData.listener);
    }

    virtual void cleanup() {
        if (signalData.listener.link.next != nullptr) {
            wl_list_remove(&signalData.listener.link);
        }
    }

    WaylandSignalData signalData;

};


#define addWaylandSignal(signal, func) internalAddSignal(std::make_unique<WaylandComponentSignal>(signal, [this](DataObject &p) {func(p);}))

}

