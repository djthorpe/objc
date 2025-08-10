/**
 * @file examples/runtime/infrared/main.c
 * @brief Test infrared receiver library
 *
 * This example demonstrates how to use the new infrared receiver library
 * with interrupt-based processing to capture IR remote control signals.
 *
 */
#include <runtime-hw/hw.h>
#include <runtime-sys/sys.h>

/////////////////////////////////////////////////////////////////////
// IR CALLBACK

/**
 * @brief IR event callback function
 *
 * This callback is invoked via interrupt whenever an IR signal event is
 * detected.
 */
void ir_event_callback(hw_infrared_event_t ir_event, uint32_t duration_us,
                       void *user_data) {

  sys_event_queue_t *queue = (sys_event_queue_t *)user_data;
  char *event = (char *)sys_malloc(50);
  if (event == NULL) {
    return;
  }

  switch (ir_event) {
  case HW_INFRARED_EVENT_MARK:
    sys_sprintf(event, 50, "MARK %u us", duration_us);
    break;
  case HW_INFRARED_EVENT_SPACE:
    sys_sprintf(event, 50, "SPACE %u us", duration_us);
    break;

  case HW_INFRARED_EVENT_TIMEOUT:
    sys_sprintf(event, 50, "TIMEOUT");
    break;
  }

  // Send the event to the queue
  sys_event_queue_push(queue, event);
}

/////////////////////////////////////////////////////////////////////
// CORE0_TASK

/**
 * @brief Function to run on core 0
 *
 * This function initializes the IR receiver library and waits for
 * interrupt-driven events.
 */
bool core0_task() {
  const uint8_t IR_PIN = 15; // IR input GPIO pin

  // Let's create a queue to receive the events
  sys_event_queue_t queue = sys_event_queue_init(10);
  if (!sys_event_queue_valid(&queue)) {
    sys_printf("Failed to create event queue\n");
    return false;
  }

  // Initialize the IR receiver
  hw_infrared_rx_t ir_receiver =
      hw_infrared_rx_init(IR_PIN, ir_event_callback, &queue);
  if (!hw_infrared_rx_valid(&ir_receiver)) {
    sys_printf("Failed to initialize IR receiver\n");
    sys_event_queue_finalize(&queue);
    return false;
  }

  // Main loop - wait for events
  while (true) {
    char *event = sys_event_queue_pop(&queue);
    if (event == NULL) {
      break;
    }

    // Process the event
    sys_puts(event);
    sys_free(event);
  }

  // Finalize the IR receiver and event queue
  hw_infrared_rx_finalize(&ir_receiver);
  sys_event_queue_finalize(&queue);

  // Return success
  return true;
}

/////////////////////////////////////////////////////////////////////
// MAIN

int main() {
  // Initialize system
  sys_init();
  hw_init();

  // Run the task
  if (core0_task() == false) {
    sys_printf("Main: Core 0 task failed\n");
  }

  // Cleanup and exit
  hw_exit();
  sys_exit();
  return 0;
}
