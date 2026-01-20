#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void my_task(void *pvParameter) {
    while(1) {
        printf("Hello from FreeRTOS task!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    // Create a task
    xTaskCreate(&my_task, "my_task", 2048, NULL, 5, NULL);
}