#pragma once

#include "common.h"

namespace wode
{

struct Dimensions {
    int width;
    int height;
};

class DataObject {
public:
    DataObject() {}
    DataObject(void *userData) { this->userData = userData; }

    template <typename T>
    operator T*() const {
        return static_cast<T*>(userData);
    }

    void *getUserData() { return userData; }
private:
    void *userData;
};

#define addSignal(signal, func) internalAddSignal(signal,  [this](DataObject &p) {func(p);})

class ComponentSignal {
public:
    ComponentSignal() {}

    virtual void cleanup() {
        
    }
};

class Component {
public:
    void internalAddSignal(std::unique_ptr<ComponentSignal> componentSignal) {
        signals.push_back(std::move(componentSignal));
    }

    void cleanup() {
        for (auto &sig : signals) {
            sig->cleanup();
        }
    }
protected:

    vector<std::unique_ptr<ComponentSignal>> signals;
};

class Compositor;

class CompositorComponent : public Component {
protected:
    CompositorComponent(Compositor &compositor);

    Compositor &compositor;
};

}
