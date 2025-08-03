#include <pico/binary_info.h>
#include <pico/unique_id.h>

// External symbols provided by the linker
extern binary_info_t *__binary_info_start;
extern binary_info_t *__binary_info_end;

///////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

const char *get_binary_info_string(uint32_t id) {
  // The symbols are addresses, not pointers that can be NULL
  // Cast them properly to get the array bounds
  binary_info_t **start = (binary_info_t **)&__binary_info_start;
  binary_info_t **end = (binary_info_t **)&__binary_info_end;

  // Safety check for valid range
  if (start >= end) {
    return NULL;
  }

  // Iterate through binary info entries
  for (binary_info_t **info_ptr = start; info_ptr < end; info_ptr++) {
    binary_info_t *info = *info_ptr;
    if (!info) {
      continue;
    }

    // Check if this is a string type with the requested ID
    if (info->type == BINARY_INFO_TYPE_ID_AND_STRING) {
      binary_info_id_and_string_t *string_info =
          (binary_info_id_and_string_t *)info;
      if (string_info->core.tag == BINARY_INFO_TAG_RASPBERRY_PI &&
          string_info->id == id) {
        return (const char *)string_info->value;
      }
    }
  }
  return NULL;
}

const char *get_program_name(void) {
  const char *name = get_binary_info_string(BINARY_INFO_ID_RP_PROGRAM_NAME);
  if (name) {
    return name;
  }

  // Fallback to compile-time macro if binary info fails
#ifdef PICO_PROGRAM_NAME
  return PICO_PROGRAM_NAME;
#elif defined(PICO_TARGET_NAME)
  return PICO_TARGET_NAME;
#else
  return "unknown";
#endif
}

const char *get_program_version(void) {
  const char *version =
      get_binary_info_string(BINARY_INFO_ID_RP_PROGRAM_VERSION_STRING);
  if (version) {
    return version;
  }

  // Fallback to compile-time macro if binary info fails
#ifdef PICO_PROGRAM_VERSION_STRING
  return PICO_PROGRAM_VERSION_STRING;
#else
  return "unknown";
#endif
}

const char *get_program_description(void) {
  const char *desc =
      get_binary_info_string(BINARY_INFO_ID_RP_PROGRAM_DESCRIPTION);
  return desc ? desc : "unknown";
}

const char *get_program_url(void) {
  const char *url = get_binary_info_string(BINARY_INFO_ID_RP_PROGRAM_URL);
  return url ? url : "unknown";
}

const char *get_build_date(void) {
  const char *date =
      get_binary_info_string(BINARY_INFO_ID_RP_PROGRAM_BUILD_DATE_STRING);
  return date ? date : "unknown";
}

const char *get_sdk_version(void) {
  const char *sdk = get_binary_info_string(BINARY_INFO_ID_RP_SDK_VERSION);
  return sdk ? sdk : "unknown";
}

const char *get_board_name(void) {
  const char *board = get_binary_info_string(BINARY_INFO_ID_RP_PICO_BOARD);
  return board ? board : "unknown";
}

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
const char *sys_env_name(void) { return get_program_name(); }

/**
 * @brief Returns the version of the current environment.
 */
const char *sys_env_version(void) { return get_program_version(); }
