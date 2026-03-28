#include <wode-wm/compositor.h>

int main(int argc, char *argv[]) {
    wlr_log_init(WLR_DEBUG, NULL);
    
    wode::Compositor c;
    c.init();

    return 0;
}
