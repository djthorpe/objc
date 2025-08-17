#include "Application+Private.h"
#include <Application/Application.h>
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS

static void _app_handle_signal(sys_env_signal_t signal);

///////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS

int NXApplicationMain(int argc, char *argv[], Class delegate,
                      NXApplicationCapability capabilities) {
  (void)argc;         // Unused parameter
  (void)argv;         // Unused parameter
  (void)capabilities; // Unused parameter

  sys_init();
  hw_init();

  // If there is no default zone, create one
  BOOL releaseZone = NO;
  NXZone *zone = [NXZone defaultZone];
  if (zone == nil) {
    // Default zone size is set to 64KB
    zone = [NXZone zoneWithSize:64 * 1024];
    if (zone == nil) {
      sys_panicf("Failed to create default zone");
      return -1;
    } else {
      releaseZone = YES;
    }
  }

  // Create an autorelease pool for memory management
  NXAutoreleasePool *pool = [[NXAutoreleasePool allocWithZone:zone] init];
  if (pool == nil) {
    sys_panicf("Failed to create autorelease pool");
    if (releaseZone) {
      [zone release];
    }
    return -1;
  }

  // Get the shared application instance
  Application *app = [Application sharedApplication];
  if (app == nil) {
    sys_panicf("No shared application instance available");
    return -1;
  }

  // Create and set the application delegate
  id<ApplicationDelegate, RetainProtocol> appDelegate = nil;
  if (delegate != nil) {
    appDelegate = [[delegate allocWithZone:zone] init];
    if (appDelegate == nil) {
      sys_panicf("Failed to create application delegate");
      if (releaseZone) {
        [zone release];
      }
      return -1;
    } else {
      [app setDelegate:appDelegate];
    }
  }

#ifdef SYSTEM_NAME_PICO
  // For Pico, we set the program name as the first argument
  NXString *programName = [NXString stringWithCString:sys_env_name()];
  [app setArgs:[NXArray arrayWithObjects:programName, nil]];
#else
  // For other systems, we create an array of arguments from argc and argv
  NXArray *args = [NXArray arrayWithCapacity:argc];
  objc_assert(args);
  int i = 0;
  for (i = 0; i < argc; i++) {
    [args append:[NXString stringWithCString:argv[i]]];
  }
  [app setArgs:args];
#endif

#ifndef SYSTEM_NAME_PICO
  // Try and set signal handlers
  sys_env_signalhandler(0, _app_handle_signal);
#endif

  // Enable the watchdog
  hw_watchdog_t *watchdog = NULL;
  if (capabilities & NXApplicationCapabilityWatchdog) {
    if (hw_watchdog_maxtimeout() > 0) {
      watchdog = hw_watchdog_init();
      if (!hw_watchdog_valid(watchdog)) {
        watchdog = NULL;
      }
    }
    if (watchdog) {
      sys_printf("Watchdog is enabled\n");
      hw_watchdog_enable(watchdog, true);
    } else {
      sys_printf("Watchdog is not supported or timer interval is too short\n");
    }
  }

  // TODO: Send signal from application when watchdog caused a reset

  // Enable power management
  // TODO: Remove this - it fits in as NXPowerManagement, not
  // a capability.
  hw_power_t *power = NULL;
  if (capabilities & NXApplicationCapabilityPower) {
    power = hw_power_init(0xFF, 0xFF, _app_power_callback, app);
    if (!hw_power_valid(power)) {
      power = NULL;
    }
  }

  // Call the run method on the shared application instance
  int returnValue = [app run];

#ifndef SYSTEM_NAME_PICO
  // Try and clear signal handlers
  sys_env_signalhandler(0, NULL);
#endif

  // Finalize watchdog and power
  hw_watchdog_finalize(watchdog);
  hw_power_finalize(power);

  // Release the application delegate, instance, pool and zone
  [app release];
  [appDelegate release];
  [pool release];
  if (releaseZone) {
    [zone release];
  }

  // Clean up the system resources
  hw_exit();
  sys_exit();

  // Return the result of the run method
  return returnValue;
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE METHODS

__attribute__((used)) static void _app_handle_signal(sys_env_signal_t signal) {
  // Although we may only get one signal at a time, we handle all
  // signals by or'ing them together, just in case....
  NXApplicationSignal appSignal = NXApplicationSignalNone;
  if (signal & SYS_ENV_SIGNAL_INT) {
    appSignal |= NXApplicationSignalInt;
  }
  if (signal & SYS_ENV_SIGNAL_TERM) {
    appSignal |= NXApplicationSignalTerm;
  }
  if (signal & SYS_ENV_SIGNAL_QUIT) {
    appSignal |= NXApplicationSignalQuit;
  }

  // Call the application's signal handler
  Application *app = [Application sharedApplication];
  if (app) {
    [app signal:appSignal];
  }
}
