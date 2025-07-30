#include <NXFoundation/NXFoundation.h>
#include <runtime-sys/sys.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

size_t NXLog(id<NXConstantStringProtocol> format, ...) {
  va_list args;
  va_start(args, format);
  size_t result = sys_vprintf([format cStr], args);
  va_end(args);
  return result;
}
