#include <pico/binary_info.h>
#include <pico/unique_id.h>

///////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

/**
 * @brief Returns a unique identifier for the current environment.
 */
const char *sys_env_serial(void) {
  static char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
  pico_get_unique_board_id_string(serial, sizeof(serial));
  return serial;
}

/**
 * @brief Returns the name of the current environment.
 */
const char *sys_env_name(void) {
#ifdef PICO_PROGRAM_NAME
  return PICO_PROGRAM_NAME;
#elif defined(PICO_TARGET_NAME)
  return PICO_TARGET_NAME;
#else
  return "pico";
#endif
}

/**
 * @brief Returns the version of the current environment.
 */
const char *sys_env_version(void) {
#ifdef PICO_PROGRAM_VERSION_STRING
  return PICO_PROGRAM_VERSION_STRING;
#else
  return "unknown";
#endif
}
