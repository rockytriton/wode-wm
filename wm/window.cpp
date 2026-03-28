#include <wode-wm/window.h>
#include <wode-wm/compositor.h>

namespace wode {

Window::Window(Compositor &c, wlr_surface *surface) : compositor(c), surface(surface) {
    
}

}
