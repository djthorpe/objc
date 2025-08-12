
///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Returns the name of the current environment.
 */
const char *sys_env_name(void) {
  const char *name = getprogname();
  return (name && *name) ? name : "unknown";
}

/**
 * @brief Returns the version of the current environment.
 */
const char *sys_env_version(void) { return "unknown"; }
