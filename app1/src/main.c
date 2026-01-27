/**
 * @file main.c
 * @brief Application 1
 * @author Aldem Pido
 */
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdio.h>


// --- Constants ---
#define LED_BUILTIN 2
#define PRINT_TIMES                                                            \
  500 // number of times to print in one cycle if FEAT_PRINT_DELAY is defined
#define BLINK_DELAY_MS 250 // LED blink delay in milliseconds
#define PRINT_DELAY_MS 10000 // Print task delay in milliseconds

// --- Feature Options ---
// #define FEAT_PRINT_DELAY
// #define FEAT_PRINT_FLOOD

/**
 * @brief A blinking LED tasks that runs at 2 Hz (500ms total)
 */
void blink_task(void *pvParameter) {
  bool state = false;                         // LED state variable
  TickType_t last_tick = xTaskGetTickCount(); // For period calculation.

  while (1) {
    // Handle LED toggle.
    state = !state;
    gpio_set_level(LED_BUILTIN, state);

    // Get tick count, display LED and Period info.
    TickType_t current_tick = xTaskGetTickCount();
    TickType_t period_ticks = current_tick - last_tick;
    last_tick = current_tick;
    uint32_t period_ms = period_ticks * portTICK_PERIOD_MS;
    printf("Security System Heartbeat %s | Period: %lu \n", (state ? "ON" : "OFF"), period_ms);

    // Delay for 250 ms.
    vTaskDelay(BLINK_DELAY_MS / portTICK_PERIOD_MS);
  }
}

/**
 * @brief A console printing task that runs at 0.1 Hz (10s)
 */
void print_task(void *pvParameter) {
  // Detrmine the number of times to print each cycle. FEAT_PRINT_DELAY
  // increases that amount.
  int print_times = 1;
#ifdef FEAT_PRINT_DELAY
  print_times = PRINT_TIMES;
#endif

  TickType_t last_tick = xTaskGetTickCount(); // For period calculation.
  while (1) {
    // Get tick count, display Period info.
    TickType_t current_tick = xTaskGetTickCount();
    TickType_t period_ticks = current_tick - last_tick;
    last_tick = current_tick;
    uint32_t period_ms = period_ticks * portTICK_PERIOD_MS;
    // Repeat print for the determined number of times.
    for (int i = 0; i < print_times; i++) {
      printf("Living Room Sensor: OK %d | Period: %lu\n", i, period_ms);
    }

    // If we define the "flood" option, while(1) will run continously and hog
    // the CPU.
#ifndef FEAT_PRINT_FLOOD
    vTaskDelay(PRINT_DELAY_MS / portTICK_PERIOD_MS);
#endif
  }
}

void app_main() {
  // Setup LED pins
  gpio_reset_pin(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
  xTaskCreate(&blink_task, "BlinkTask", 2048, NULL, 1, NULL);
  xTaskCreate(&print_task, "PrintTask", 2048, NULL, 2, NULL);
}