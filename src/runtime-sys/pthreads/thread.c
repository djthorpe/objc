/**
 * @file thread.c
 * @brief pthread-based thread management implementation
 *
 * This file implements thread management functions using POSIX pthreads.
 * Threads are created as detached to implement fire-and-forget semantics.
 */

#ifdef __linux__
#define _GNU_SOURCE
#endif

#include "runtime-sys/thread.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

#ifdef __linux__
#include <sched.h>
#include <sys/sysinfo.h>
#endif

/**
 * @brief Thread wrapper structure for pthread compatibility
 */
typedef struct {
  sys_thread_func_t func;
  void *arg;
} thread_wrapper_t;

/**
 * @brief Wrapper function to adapt sys_thread_func_t to pthread function
 * signature
 */
static void *thread_wrapper(void *arg) {
  thread_wrapper_t *wrapper = (thread_wrapper_t *)arg;
  sys_thread_func_t func = wrapper->func;
  void *thread_arg = wrapper->arg;

  // Free the wrapper structure
  free(wrapper);

  // Call the actual thread function
  func(thread_arg);

  return NULL;
}

uint8_t sys_thread_numcores(void) {
#ifdef __APPLE__
  int mac_cores;
  size_t len = sizeof(mac_cores);
  if (sysctlbyname("hw.ncpu", &mac_cores, &len, NULL, 0) == 0) {
    return (uint8_t)(mac_cores > 255 ? 255 : mac_cores);
  }
#elif defined(__linux__)
  int linux_cores = get_nprocs();
  if (linux_cores > 0) {
    return (uint8_t)(linux_cores > 255 ? 255 : linux_cores);
  }
#endif

  // Fallback to sysconf if available
  long sysconf_cores = sysconf(_SC_NPROCESSORS_ONLN);
  if (sysconf_cores > 0) {
    return (uint8_t)(sysconf_cores > 255 ? 255 : sysconf_cores);
  }

  return 1; // Default to 1 core if detection fails
}

bool sys_thread_create(sys_thread_func_t func, void *arg) {
  if (!func) {
    return false;
  }

  // Allocate wrapper structure
  thread_wrapper_t *wrapper = malloc(sizeof(thread_wrapper_t));
  if (!wrapper) {
    return false;
  }

  wrapper->func = func;
  wrapper->arg = arg;

  pthread_t thread;
  pthread_attr_t attr;

  // Initialize thread attributes
  if (pthread_attr_init(&attr) != 0) {
    free(wrapper);
    return false;
  }

  // Set thread to detached state for fire-and-forget behavior
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    pthread_attr_destroy(&attr);
    free(wrapper);
    return false;
  }

  // Create the thread
  int result = pthread_create(&thread, &attr, thread_wrapper, wrapper);

  // Clean up attributes
  pthread_attr_destroy(&attr);

  if (result != 0) {
    free(wrapper);
    return false;
  }

  return true;
}

bool sys_thread_create_on_core(sys_thread_func_t func, void *arg,
                               uint8_t core) {
  if (!func) {
    return false;
  }

  // Check if the requested core is valid
  uint8_t num_cores = sys_thread_numcores();
  if (core >= num_cores) {
    return false;
  }

  // Allocate wrapper structure
  thread_wrapper_t *wrapper = malloc(sizeof(thread_wrapper_t));
  if (!wrapper) {
    return false;
  }

  wrapper->func = func;
  wrapper->arg = arg;

  pthread_t thread;
  pthread_attr_t attr;

  // Initialize thread attributes
  if (pthread_attr_init(&attr) != 0) {
    free(wrapper);
    return false;
  }

  // Set thread to detached state for fire-and-forget behavior
  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
    pthread_attr_destroy(&attr);
    free(wrapper);
    return false;
  }

  // Create the thread
  int result = pthread_create(&thread, &attr, thread_wrapper, wrapper);

  if (result == 0) {
    // Try to set CPU affinity after thread creation
    // Note: This is platform-specific and may not be available on all systems
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    // Attempt to set affinity, but don't fail if it's not supported
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
#elif defined(__APPLE__)
    // macOS doesn't support pthread CPU affinity directly
    // The thread will run on any available core
    // For true core affinity on macOS, would need thread_policy_set()
    // but that's much more complex and not always reliable
#endif
  }

  // Clean up attributes
  pthread_attr_destroy(&attr);

  if (result != 0) {
    free(wrapper);
    return false;
  }

  return true;
}

uint8_t sys_thread_core(void) {
#ifdef __linux__
  // On Linux, use sched_getcpu() to get current CPU
  int cpu = sched_getcpu();
  if (cpu >= 0) {
    return (uint8_t)(cpu > 255 ? 255 : cpu);
  }
#elif defined(__APPLE__)
  // On macOS, there's no direct way to get the current CPU core
  // We could use thread_info() but it's complex and not always reliable
  // For now, return 0 as a fallback
  // Note: Could potentially use:
  // - thread_info() with THREAD_BASIC_INFO
  // - sysctlbyname() with specific queries
  // But these are not straightforward for getting current CPU

  // Fallback: assume core 0 on macOS for simplicity
  return 0;
#endif

  // Default fallback for other platforms or if detection fails
  return 0;
}
