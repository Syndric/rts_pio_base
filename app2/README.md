
# Application 2

Aldem Pido
2/10/26

[RTS-SP26-Application-02-Aldem-Pido - Wokwi ESP32, STM32, Arduino Simulator](https://wokwi.com/projects/455539829667384321)

## AI Statement

AI was used to double check these answers in this readme. All answers were fully written out, and then this whole README document was put into Gemini for suggestions. VSCode Copilot auto-completion with tab was also used for the initial code. No agents were used to generate the code. 

## 1

I assigned the task priorities as follows:

* Sensor task: 3
* LED Blink: 2
* Console print: 1 (logger)

Each task's period was perfectly regular in practice. For the sensor task, the sensor readings and alert messages occurred at a consistent 500ms. I did not observe any drift or variation in the LED blink or print intervals, and they also stayed at a consistent 500/1000ms. This behavior is attributed to the fact that all of our tasks require very little computation.

Jitter may happen if the sensor task wakes up at the same moment as the LED task. In this case, the LED task will have to wait.

`vTaskDelayUntil()` provides a more stable period for the sensor task because it bases the next wake-up tick on an absolute time reference, rather than how `vTaskDelay()` calculates based on a relative time reference. Thus, the actual period for `vTaskDelay()` is *500ms + execution time.* `vTaskDelayUntil()` corrects this.

## 2

In FreeRTOS, priority-based preemptive scheduling is used. If the console task is about to print, but the sensor's task period arrives, the Sensor task will interrupt the print task immediately. Once the Sensor task finishes, the console print task will resume.

Tick 1 - Console Task is RUNNING.
Tick 2 - Sensor timer expires, moves from BLOCKED to READY. Scheduler preempts Console task.
Tick 3 - Sensor task executes its task.
Tick 4 - Sensor task calls `vTaskDelayUntil()` and enters blocked state. Console task resumes as it is the highest priority READY task.

## 3

If the sensor task's execution time exceeds its period, it will likely starve the other tasks as it uses `vTaskDelayUntil()`. For example, if a task with a 500ms period takes 600ms to run, the CPU utilization for that task is 120%, which would result in a deadline miss. We would expect to see delayed print functions and delayed LED toggles. A system designer should use a relative period (`vTaskDelay()`), increase the period, move it to a lower priority, or move it to a different core.

## 4

We chose `vTaskDelayUntil()` instead of `vTaskDelay()` to ensure that the task period does not drift. If we used `vTaskDelay()`, the task readings might drift by a few milliseconds. For sensor sampling, consistent time periods are required for accuracy. The LED blink task is fine to drift because it is only a status indicator and not essential.

## 5

Healthcare:

The sensor could represent a heartbeat monitor. If the heartbeat monitor is drifting, it will result in incorrect BPM readings. For algorithms that analyze heartbeats, This will have the highest priority.

The led indicator could just be a simple indicator that the device is working.

The logger only collects telemetry, and isn't meant to be normally used. Thus, it can have the lowest priority.
