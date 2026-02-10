/* --------------------------------------------------------------
   Application: 02 - Rev1
   Release Type: Baseline Preemption
   Class: Real Time Systems - Sp 2026
   Author: [M Borowczak, Aldem Pido] 
   Email: [mike.borowczak@ucf.edu, aldem.pido@ucf.edu]
   Company: [University of Central Florida]
   Website: theDRACOlab.com
   AI Use: Commented inline -- None
---------------------------------------------------------------*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "math.h"

#define LED_PIN GPIO_NUM_2  // Using GPIO2 for the LED
#define LDR_PIN GPIO_NUM_32 // Analog pin for LDR sensor input
#define LDR_ADC_CHANNEL ADC1_CHANNEL_4 // ADC1_CHANNEL_4 corresponds to GPIO32 on ESP32, adjust if using a different pin

#define AVG_WINDOW 10 // Size of the moving average window for sensor readings
#define SENSOR_THRESHOLD 500 // Threshold for triggering an alert based on the average sensor reading; adjust based on context and experiments

// LED at 1 Hz (1000 ms period: 500 ms ON, 500 ms OFF);
void led_task(void *pvParameters) {
    bool led_status = false;
    TickType_t currentTime = pdTICKS_TO_MS( xTaskGetTickCount() );

    while (1) {
        currentTime = pdTICKS_TO_MS( xTaskGetTickCount() );
        gpio_set_level(LED_PIN, led_status ? 1 : 0);  // Set LED pin high or low based on led_status flag;
        led_status = !led_status;  // toggle state for next loop 
        
        printf("Patient sensor indicator: %s @ %lu\n", led_status ? "ON" : "OFF", currentTime);
        vTaskDelay(pdMS_TO_TICKS(500)); // Delay for 500 ms using MS to Ticks Function vs alternative which is MS / ticks per ms
       
    
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

// Task to print a message every 1000 ms (1 seconds)
void print_status_task(void *pvParameters) {
    TickType_t currentTime = pdTICKS_TO_MS( xTaskGetTickCount() );
    TickType_t previousTime = 0;
    uint16_t counter = 0;
    while (1) {
        previousTime = currentTime;
        currentTime = pdTICKS_TO_MS( xTaskGetTickCount() );
        counter++;
        
        // Prints a periodic message based on a thematic area. Output a timestamp (ms) and period (ms)
        printf("Heartbeat alive, count is %d and time is %lu\n",counter, currentTime);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 ms
    }
    vTaskDelete(NULL); // We'll never get here; tasks run forever
}

// Create new task for sensor reading every 500ms
void sensor_task(void *pvParameters) {
    // Variables to compute LUX
    int raw;
    float Vmeasure = 0.;
    float Rmeasure = 0.;
    float lux = 0.;
    // Variables for moving average
    float luxreadings[AVG_WINDOW] = {0};
    int idx = 0;
    float sum = 0;

    // Pre-fill the readings array with an initial sample to avoid startup anomaly
    for(int i = 0; i < AVG_WINDOW; ++i) {
        raw =  adc1_get_raw(LDR_ADC_CHANNEL);
        Vmeasure = raw/4095.0 * 3.3; //TODO11e correct this with the equation seen earlier
        Rmeasure = (10000 * Vmeasure / 3.3) / (1 - (Vmeasure / 3.3)); //TODO11f correct this with the equation seen earlier
        lux = pow(50*1000*pow(10,0.7)/Rmeasure, 1/0.7); //TODO11g correct this with the equation seen earlier
        luxreadings[i] = lux;
        sum += luxreadings[i];
    }

    const TickType_t periodTicks = pdMS_TO_TICKS(500); // e.g. 500 ms period
    TickType_t lastWakeTime = xTaskGetTickCount(); // initialize last wake time

    while (1) {
        // Read current sensor value
        raw = adc1_get_raw(LDR_ADC_CHANNEL);
        //printf("**raw **: Sensor %d\n", raw);

        // Compute LUX 
        Vmeasure = raw/4095.0 * 3.3; //TODO11e correct this with the equation seen earlier
        Rmeasure = (10000 * Vmeasure / 3.3) / (1 - (Vmeasure / 3.3)); //TODO11f correct this with the equation seen earlier
        if(Rmeasure < 1e-6) {
            Rmeasure = 1e-6; // Prevent division by zero or very small resistance values
        }
        lux = pow(50*1000*pow(10,0.7)/Rmeasure, 1/0.7); //TODO11g correct this with the equation seen earlier
       
        // Update moving average buffer 
        sum -= luxreadings[idx];       // remove oldest value from sum
        
        luxreadings[idx] = lux;        // place new reading
        sum += lux;                 // add new value to sum
        idx = (idx + 1) % AVG_WINDOW;
        float avg = sum / AVG_WINDOW; // compute average

        // Check threshold and print alert if exceeded or below based on context
        if (avg > SENSOR_THRESHOLD) {
            printf("**Caretaker Alert**: Sensor average %.2f exceeds threshold %d! @ %lu\n", avg, SENSOR_THRESHOLD, pdTICKS_TO_MS( xTaskGetTickCount() ));
        } else {
          printf("Average value: Sensor average %.2f @ %lu\n", avg, pdTICKS_TO_MS( xTaskGetTickCount() ));
        }
        //TODO11j: Print out time period [to help with answering Eng/Analysis quetionst (hint check Application Solution #1 )
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500));

    }
}


void app_main() {
    // Initialize LED GPIO     
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Initialize LDR PIN as INPUT [2 lines mirroring those above]
    gpio_reset_pin(LDR_PIN);
    gpio_set_direction(LDR_PIN, GPIO_MODE_INPUT);

    // Set ADC1's resolution by calling:
    adc1_config_width(ADC_WIDTH_BIT_12);


    // Set the the input channel to 11 DB Attenuation using
    // function adc1_config_channel_atten(...,...) 
    // with parameters LDR_ADC_CHANNEL and ADC_ATTEN_DB_11
    adc1_config_channel_atten(LDR_ADC_CHANNEL, ADC_ATTEN_DB_11);


    // LED task is given medium priority
    xTaskCreatePinnedToCore(led_task, "LED", 2048, NULL, 2, NULL, 1);
    // Print status is given low priority (a logger)
    xTaskCreatePinnedToCore(print_status_task, "STATUS", 2048, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(sensor_task, "SensorTask", 4096, NULL, 3, NULL, 1);

}
