#include <pico/platform/panic.h>
#include <runtime-sys/sys.h>

void sys_abort(void) { panic("\nHALT"); }
