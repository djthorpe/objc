#define _GNU_SOURCE
#include <errno.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Returns the name of the current environment.
 */
const char *sys_env_name(void) {
  const char *name = program_invocation_short_name;
  return (name && *name) ? name : "unknown";
}

/**
 * @brief Returns the version of the current environment.
 */
const char *sys_env_version(void) { return "unknown"; }
