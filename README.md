# avr-simple-scheduler
A really simple, non-preemptive task scheduler. 

### Table of Contents
- [Introduction](#introduction)
- [How it works](#how-it-works)
- [About the demo](#about-the-demo)

### Introduction
This is a super simple, non-preemptive, task scheduler. This demo project was made in Atmel Studio 7 for ATmega48P microcontroller, but can be ported easily.<br />
The point of this scheduler is to call and execute every task with a selected periodicity.

### How it works
The scheduler is inside the OS folder (OS.c and OS.h).
Although it isn't an operating system I still call it OS, because the naming is easier this way (OS is shorter, than Scheduler, duh).<br />
There are three main, important functions:
- **OS_TaskCreate()** is the function, that registers the tasks. The inputs are the task's function, the task's burst time (the time you want to call it periodically) and the task’s default state for special needs.
<br><img src="https://raw.githubusercontent.com/ferenc-nemeth/avr-simple-scheduler/master/Design/TaskCreate.png" ><br>
*Figure 1. Activity diagram of OS_TaskCreate().*

There are three states: 
  - BLOCKED: waits for a call from timer
  - READY: ready for execution
  - SUSPENDED: disabled, won't do anything
  <br><img src="https://raw.githubusercontent.com/ferenc-nemeth/avr-simple-scheduler/master/Design/States.png" ><br>
  *Figure 2. State diagram of the tasks.*
  
- **OS_TaskTimer()** is the function, that organizes the tasks. It should be in a timer interrupt (in the demo it is a 1 sec timer, but can be anything). There is a counter inside the function. If the counter is bigger, then the task's burst time, it puts it into READY state.
<br><img src="https://raw.githubusercontent.com/ferenc-nemeth/avr-simple-scheduler/master/Design/TaskTimer.png" ><br>
*Figure 3. Activity diagram of OS_TaskTimer().*

- **OS_TaskExecution()** is the function, that calls the tasks. It should be in the main() infinite loop. It checks if there are any READY tasks, and then executes them in a FIFO style. It is worth to mention that because of this, it is just soft real time. If you choose the timings wrong, then there might be too many tasks in READY state and unwanted delays might happen.
<br><img src="https://raw.githubusercontent.com/ferenc-nemeth/avr-simple-scheduler/master/Design/TaskExecution.png" ><br>
*Figure 4. Activity diagram of OS_TaskExecution().*

### About the demo
The demo is intended to demonstrate how the scheduler works. There are three tasks and each task blinks a LED (on port D 0,1,2) with 1, 2 and 5 seconds. The second task starts in SUSPENDED state, so it never gets executed.
