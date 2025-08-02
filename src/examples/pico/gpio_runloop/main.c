/**
 * @file main.c
 * @brief Pico event queue with GPIO events example
 *
 * This example demonstrates how to inject events into a system event queue
 * from GPIO interrupts on the RP2040/RP2050 (Pico) platform.
 *
 * We call shutdown after the BOOTSEL button is pressed, and
 * the consumers on each core will end when all events have been drained.
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

#define GPIO_BOOTSEL 23 // Pin used for BOOTSEL button
#define GPIO_A 12       // Pin A
#define GPIO_B 13       // Pin B
#define GPIO_C 14       // Pin C

/////////////////////////////////////////////////////////////////////
// RUNLOOP

void runloop(sys_event_queue_t *queue) {
  int core = sys_thread_core();
  sys_printf("Starting runloop on core %d...\n", core);

  // Process events until the queue is empty
  while (true) {
    sys_event_t event = sys_event_queue_pop(queue);
    if (event == NULL) {
      // No more events to process
      break;
    }

    // Simulate processing the event
    sys_printf("core %d: Processing event: %s (queue size=%d)\n", core,
               (char *)event, sys_event_queue_size(queue));

    // Free the allocated string
    sys_free(event);
  }
  sys_printf("core %d: Runloop finished.\n", core);
}

/////////////////////////////////////////////////////////////////////
// GPIO CALLBACK

void gpio_callback(uint8_t pin, hw_gpio_event_t event, void *userdata) {
  sys_event_queue_t *queue = (sys_event_queue_t *)userdata;
  int core = sys_thread_core();

  // Shutdown the queue if we have produced enough events
  if (pin == GPIO_BOOTSEL) {
    sys_event_queue_shutdown(queue);
    return;
  }

  // Put the event into the queue
  // Create a new event string
  char *event_data = sys_malloc(50);
  if (!event_data) {
    return;
  } else {
    sys_sprintf(event_data, 50, "gpio(core=%d pin=%d state=%s)", core, pin,
                (event == HW_GPIO_RISING ? "RISING" : "FALLING"));
  }

  // Push the event into the queue
  if (!sys_event_queue_try_push(queue, event_data)) {
    sys_free(event_data);
  }
}

/////////////////////////////////////////////////////////////////////
// CORE 1 TASKS

/**
 * @brief Function to run on core 1
 *
 * This function is executed on core 1 of the RP2040 (or just simply "another
 * thread" on other platforms).
 *
 * A runloop is run until there are no more events to process.
 */
void core1_task(void *arg) {
  sys_event_queue_t *queue = (sys_event_queue_t *)arg;
  sys_printf("core %d: Starting task\n", sys_thread_core());
  runloop(queue);
  sys_printf("core %d: Task completed!\n", sys_thread_core());
}

/////////////////////////////////////////////////////////////////////
// CORE 0 TASKS

/**
 * @brief Function to run on core 0
 *
 * This function creates a queue, initializes a GPIO pin for input,
 * and processes events from the queue.
 */
bool core0_task() {
  // Initialize the event queue with a capacity of 20 events
  sys_event_queue_t queue = sys_event_queue_init(20);

  // Create the task on core 1. If this wasn't an RP2040, we would
  // just use sys_thread_create() instead.
  sys_printf("main: Starting task on core 1...\n");
  if (sys_thread_create_on_core(core1_task, &queue, 1) == false) {
    sys_printf("main: Failed to launch task on core 1\n");
    return false;
  }

  // Initialize a GPIO pin for input
  hw_gpio_init(GPIO_A, HW_GPIO_PULLUP);
  hw_gpio_init(GPIO_B, HW_GPIO_PULLUP);
  hw_gpio_init(GPIO_C, HW_GPIO_PULLUP);

  // The BOOTSEL button is used to signal shutdown
  hw_gpio_init(GPIO_BOOTSEL, HW_GPIO_INPUT);

  // Set the GPIO callback for injecting events into the queue
  hw_gpio_set_callback(gpio_callback, &queue);

  // Run the event processing loop
  runloop(&queue);

  sys_printf("core %d: Task completed!\n", sys_thread_core());
  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize
  sys_init();
  hw_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: core 0 task failed\n");
  }

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
