#if defined __APPLE__  && defined __aarch64__
#include "main-arm64-darwin.s"
#elif defined __linux__ && defined __aarch64__ 
#include "main-arm64-linux.s"
#else
#pragma message "Unsupported os/arch combination"
#endif
