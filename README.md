# ARM-and-RTOS-implementation-of-SCADA-System
## Introduction to SCADA System
A power plant operator uses a SCADA system for safe operations. The system continuously monitors the rate of flow of air and fuel (natural gas) into the combustion chamber, the speed of the turbine, and the temperature in the combustion chamber. If the speed of the turbine or the temperature in the combustion chamber reach values outside the safe operating range, then it shuts down the unit. Note that a power plant, like most industrial units, cannot be abruptly shut down. It must undergo a controlled shutdown.

## SCADA system
SCADA, or supervisory control and data acquisition, is a potent technology used to manage the processing and visualization of data from industrial equipment. Operators and field supervisors can obtain actionable data and manage hundreds of assets using a modern SCADA solution without having to visit every field. This can result in a considerable cost savings.
<br>
## Implementation Details
### Requirements
* LPC2148 Microcontroller (Based on ARM7TDMI processor 32-bits)
* Keil- Micro Vision Software
* LED's (Indicating each sectors)
* Buzzer ( Used as alarm during Shutdown)
* LCD (to display warning message
* ADC Knob (to vary the temperature)
* UART (to vary speed of turbine)

### Conditions
* Threshold temperature - If temperation goes above the range of 200 degree then system will shut down
* Threshold speed - If speed of turbin goes above it then system will shut down(say 8rpm)

### Working
We implemented 3 function
1. temp_monitoring_system()
    - This function continuosly monitors the temperature of the air as the temperature is varied through ADC knob
2. speed_monitor_system()
    - This continously monitors the speed of the turbine as speed in varied through UART.
3. shutdown_system()
    - This system is responsible for qradually shutting down the system
    - Buzzer buzzes as alarm indicating problem in the power plant
    - LED's will be shutting down one after the other.

### Problem with this System
* Here we assume that when are system is monitoring either speed or temperature then other system is in the safe operating zone.
* But in real time it is required that we have to simultaneously monitor both  speed and temperature.

## RTOS Implenmentation
Kernel used RTX (Real Time Execution)
In order to overcome drawbacks of ARM implementation we switched to the real time operating system.
Here we use CPU scheduling algorithms to solve the problem. The Round Robin algorithm with preemptive priority based scheduling is used.
Kernel objects used are Tasks.

### Construction
- Hardware utilisation remains the same as in ARM implementation.
- Here Tasks are created instead of functions as in ARM implementation.

* Speed_Monitor_task
*Temperature_monitor_task
* Shutdown_task
