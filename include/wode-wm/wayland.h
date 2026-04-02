#pragma once

#include <wode-wm/wode.h>
#include <wode-wm/wlr.h>

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

