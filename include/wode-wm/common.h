#pragma once


#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <type_traits>

#include <functional>

#include <print>
using std::println;

#include <memory>
using std::make_unique;
using std::unique_ptr;

#include <vector>
using std::vector;

#include <xkbcommon/xkbcommon.h>

enum CursorMode {
	TINYWL_CURSOR_PASSTHROUGH,
	TINYWL_CURSOR_MOVE,
	TINYWL_CURSOR_RESIZE,
};
