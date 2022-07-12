# ARM-and-RTOS-implementation-of-SCADA-System
## Introduction to SCADA System
A power plant operator uses a SCADA system for safe operations. The system continuously monitors the rate of flow of air and fuel (natural gas) into the combustion chamber, the speed of the turbine, and the temperature in the combustion chamber. If the turbine's speed or the combustion chamber's temperature reaches values outside the safe operating range, it shuts down the unit. Note that a power plant, like most industrial units, cannot be abruptly shut down. It must undergo a controlled shutdown.

## SCADA system
SCADA, or supervisory control and data acquisition, is a powerful technology used to manage the processing and visualization of data from industrial equipment. Operators and field supervisors can obtain actionable data and manage hundreds of assets using a modern SCADA solution without having to visit every field. This can result in considerable cost savings.
<br>
## Implementation Details
### Requirements
* LPC2148 Microcontroller (Based on ARM7TDMI processor 32-bits)
* Keil- Micro Vision Software
* LED's (Indicating each sector)
* Buzzer ( Used as alarm during Shutdown)
* LCD (to display the warning message
* ADC Knob (to vary the temperature)
* UART (to vary the speed of turbine)

### Conditions
* Threshold temperature - If the temperature goes above the range of 200 degrees then the system will shut down
* Threshold speed - If the speed of the turbine goes above it then the system will shut down(say 8rpm)

### Working
We implemented 3 function
1. temp_monitoring_system()
    - This function continuously monitors the temperature of the air as the temperature is varied through the ADC knob
2. speed_monitor_system()
    - This continuously monitors the speed of the turbine as speed is varied through UART.
3. shutdown_system()
    - This system is responsible for gradually shutting down the system
    - Buzzer buzzes as an alarm indicating a problem in the power plant
    - LEDs will be shutting down one after the other.

### Problem with this System
* Here we assume that when our system is monitoring either speed or temperature then another system is in the safe operating zone.
* But in real-time it is required that we have to simultaneously monitor both speed and temperature.

## RTOS Implementation
The kernel used RTX (Real-Time Execution)
To overcome the drawbacks of ARM implementation we switched to the real-time operating system.
Here we use CPU scheduling algorithms to solve the problem. The Round Robin algorithm with preemptive priority-based scheduling is used.
Kernel objects used are Tasks.

### Construction
Hardware utilization remains the same as in ARM implementation.
Here Tasks are created instead of functions as in ARM implementation.

* Speed_Monitor_task
* Temperature_monitor_task
* Shutdown_task


### Working
* First Task_1 is created with some priority. this task is to monitor the temperature of the combustion chamber as we give inputs through the ADC knob.
* This task checks whether the temperature is greater than the threshold temperature.
* If yes then creates Task_3 with a priority higher than Task_1, i.e, to perform the gradual shutdown of the system.
* If no then creates Task_2 with its priority higher than Task_1, i.e., to monitor the speed of the turbine.
* In Task_2 it checks for the speed of the turbine, if the speed crosses the threshold speed (8rps), then the priority of Task_3 is increased and the shutdown process begins.
* Until the threshold speed, is not crossed the priority of Task_2 is decreased lower than the Task_1 so that the temperature monitor comes into the picture.

Hence one task will prompt to run another task based on the priority.
Now consider if both speed and temperature are within the safe operating region the CPU will switch between Task_1 and Task_2 is so frequent that it appears to run simultaneously.

