#include <runtime-sys/sys.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

static sys_env_signal_callback_t _sys_env_signal_callback_ex = NULL;

static void _sys_env_signal_callback(int signo) {
  if (_sys_env_signal_callback_ex != NULL) {
    switch (signo) {
    case SIGTERM:
      _sys_env_signal_callback_ex(SYS_ENV_SIGNAL_TERM);
      break;
    case SIGINT:
      _sys_env_signal_callback_ex(SYS_ENV_SIGNAL_INT);
      break;
    case SIGQUIT:
      _sys_env_signal_callback_ex(SYS_ENV_SIGNAL_QUIT);
      break;
    default:
      // Ignore other signals
      return;
    }
  }
}

bool sys_env_signalhandler(sys_env_signal_t mask,
                           sys_env_signal_callback_t callback) {
  bool success = true;

  // Clear previous signal handlers
  if (mask & SYS_ENV_SIGNAL_TERM || mask == 0) {
    if (signal(SIGTERM, SIG_DFL) == SIG_ERR) {
      success = false; // Failed to reset handler
    }
  }
  if (mask & SYS_ENV_SIGNAL_INT || mask == 0) {
    if (signal(SIGINT, SIG_DFL) == SIG_ERR) {
      success = false; // Failed to reset handler
    }
  }
  if (mask & SYS_ENV_SIGNAL_QUIT || mask == 0) {
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
      success = false; // Failed to reset handler
    }
  }

  // Clear the global callback
  _sys_env_signal_callback_ex = NULL;

  // Set up new signal handlers
  if (callback != NULL) {
    if (mask & SYS_ENV_SIGNAL_TERM || mask == 0) {
      if (signal(SIGTERM, _sys_env_signal_callback) == SIG_ERR) {
        success = false; // Failed to set handler
      }
    }
    if (mask & SYS_ENV_SIGNAL_INT || mask == 0) {
      if (signal(SIGINT, _sys_env_signal_callback) == SIG_ERR) {
        success = false; // Failed to set handler
      }
    }
    if (mask & SYS_ENV_SIGNAL_QUIT || mask == 0) {
      if (signal(SIGQUIT, _sys_env_signal_callback) == SIG_ERR) {
        success = false; // Failed to set handler
      }
    }

    // Set up signal handling
    if (success) {
      _sys_env_signal_callback_ex = callback;
    }
  }
  return success;
}
