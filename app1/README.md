# Application 1

[Wowki link](https://wokwi.com/projects/454329859073155073)
Note: LED GPIO and color changed only in the Wowki Project.

## Directory

Aldem Pido
1/27/26

## Objective

The goal is to implement multiple tasks using different modes of output at different refresh rates. To do this, blinking LED and console print tasks will be implemented.

## Questions

### Q1

Nothing "bad" will happen if the delay within the print task is increased.
If the number of characters is too much, then because PrintTask has a higher priority than BlinkTask, BlinkTask will not run until PrintTasks finishes its execution.

### Q2

In this scenario, the LED period varies between 250ms-1000ms.

### Q3

In both healthcare and security scenarios, for our target users, we want our users to only see what they need to see. For healthcare, we would only want to print the heartrate, or a status indicator of the heartrate sensor wasn't working. For security, we would want to print the status of each overall system.

Having the correct functionality in these systems is beneficial because it allows the users to know what is "normal", so when something does go wrong, they know when it does.

### Q4

a. Periods measured by printing periods between last calls using `TickType_t` and `xTaskGetTickCount()`

b. LED blink task period: 250ms consistently

c. Print task period: 10000ms consistently

### Q5

We do not know if our system tasks meet the timing requirements as no timing requirements were given. However, our periods are not deviating from the given number so we can assume that we would meet those timing requirements.

Periods were verified via the terminal output.

### Q6

I ran the code when the printing message was repeated 500 times in a for loop.
The LED missed its schedule 250ms cycle, ranging from 250ms - 1000ms.

### Q7

Yes, we were able to achieve timing determinism for the LED blink. It consistently blinks at 250ms, and we know it will blink at 250ms.

### Other

Theme chosen: security system

To implement two tasks with specific delays, a single-loop approach would to choose a "base" delay time and have a delay counter that increments every delay period. Then, certain functions can run for certain values of the delay counter.

A task-based approach splits the single loop into multiple different loops so that their delay timers are independent from each other, real-time, and can prioritize different tasks.

Proof of output:
![Proof of output](assets/output_proof_1_27_26.png)

### AI Statement
Copilot was used to generate the code for getting period count. AI was not used for anything else in this folder.
