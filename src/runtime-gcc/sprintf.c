#include <objc/objc.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/sys.h>

enum _objc_sprintf_flags {
  OBJC_PRINTF_LONG = 1 << 0,
  OBJC_PRINTF_HEX = 1 << 1,
  OBJC_PRINTF_BIN = 1 << 2,
  OBJC_PRINTF_UPPER = 1 << 3,
  OBJC_PRINTF_NEG = 1 << 4,
  OBJC_PRINTF_DONE = 1 << 5
};

struct _objc_sprintf_state {
  char *buf;                      ///< Output buffer
  size_t sz;                      ///< Size of the output buffer
  size_t in;                      ///< Current index in the format buffer
  size_t out;                     ///< Current index in the output buffer
  size_t len;                     ///< Length of the formatted string
  enum _objc_sprintf_flags flags; ///< Flags
};

/**
 * @brief Append a character into the buffer
 */
static inline void _objc_printf_chtostr(struct _objc_sprintf_state *state,
                                        const char ch) {
  objc_assert(state);

  // Output the character if we have a buffer and space
  if (state->buf && state->out < (state->sz - 1)) {
    state->buf[state->out] = ch;
    state->out++;
  }
  state->len++;
}

/**
 * @brief Format a string into the output buffer, replacing
 * formatting directives with a string representation of the
 * provided arguments.
 */
static size_t _objc_vsprintf_new(struct _objc_sprintf_state *state,
                                 const char *format, va_list va) {
  objc_assert(format);
  objc_assert(state);
  objc_assert(state->buf == NULL || state->sz > 1);
  objc_assert((state->flags & OBJC_PRINTF_DONE) == 0);

  while (state->out < (state->sz - 1) &&
         (state->flags & OBJC_PRINTF_DONE) == 0) {
    char ch = format[state->in++];
    switch (ch) {
    case '\0':
      state->flags |= OBJC_PRINTF_DONE;
      break;
    default:
      _objc_printf_chtostr(state, ch);
    }
  }

  // Terminate the string if we reached the end of the format
  if (state->buf) {
    state->buf[state->out] = '\0';
    state->out = 0;
  }

  // Reset the out counter

  // Return current length of the string
  return state->len;
}

/**
 * @brief Public function to print a formatted string with variable arguments
 */
size_t objc_printf(const char *format, ...) {
  objc_assert(format);
  va_list args;
  va_start(args, format);
  static char _objc_printf_buf[80];
  struct _objc_sprintf_state state = {.buf = _objc_printf_buf,
                                      .sz = 80,
                                      .in = 0,
                                      .out = 0,
                                      .len = 0,
                                      .done = NO};
  do {
    size_t sz = _objc_vsprintf_new(&state, format, args);
    sys_puts(_objc_printf_buf);
  } while (!state.done);
  va_end(args);
  return state.len;
}
