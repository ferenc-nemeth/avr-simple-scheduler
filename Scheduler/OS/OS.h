/**
* @file    OS.h
* @author  Ferenc Nemeth
* @date    21 Jul 2018
* @brief   This is a really simple, non-preemptive task scheduler.
*          You can register tasks with their runnable function and the periodic time you want to call them.
*          With a help of a timer the tasks get into READY state after every time period (except if they are SUSPENDED) and
*          they get called and executed in the main()'s inifinte loop. After they are finished everything starts over.
*          This Scheduler helps you to keep your tasks and timing organized.
*
*          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth/
*/

#ifndef OS_H_
#define OS_H_

#include <avr/io.h>
#include <stddef.h>

#define OS_MAX_TASK_NUM			((uint8_t)0x10)  /**< Maximal task number that can be registered. */

#define OS_CHECK_TIME_LIMITS	false
#define OS_MAX_TIME				((uint8_t)10u)  /**< Maximal time that a task can run (OS_MAX_TIME*time_ticks). */
#define OS_MIN_TIME     		((uint8_t)1u)   /**< Minimal time that a task can run (OS_MIN_TIME*time_ticks). */

//typedef void (*fncPtr)(void);           /**< Function pointer for registering tasks. */
typedef void (*OS_task_func_t)(uint8_t task_num);           /**< Function pointer for registering tasks. */

/**
* States of the tasks.
*/
typedef enum
{
	BLOCKED = 0,                        /**< In the BLOCKED state the task waits for the timer to put it into READY state. */
	READY,                              /**< In the READY state the task is ready to be called and executed in the main function. */
	SUSPENDED                           /**< In the SUSPENDED state the task is ignored by the timer and executer. */
} OS_state;

/**
* Variables of the tasks.
*/
typedef struct
{
	OS_task_func_t	function;               /**< This is the task that gets called periodically. */
	const char*		task_name;
	uint8_t			time_burst;             /**< The period we want to call it. */
	uint8_t			time_cnt;               /**< Counter, if it reaches the time_burst, then the timer puts it into READY state. */
	OS_state		state;                  /**< The current state of the task. */
} OS_struct;

/**
* Feedback and error handling for the task creation.
*/
typedef enum
{
	OS_OK = 0,                             /**< OK:    Everything went as expected. */
	OS_NOK_NULL_PTR,                       /**< ERROR: Null pointer as a task. */
	OS_NOK_TIME_LIMIT,                     /**< ERROR: The time period is more or less, than the limits. */
	OS_NOK_CNT_LIMIT,                      /**< ERROR: Something horrible happened, time to panic! */
	OS_NOK_UNKNOWN,
	OS_NOT_FOUND
} OS_status_t;

typedef struct
{
	uint8_t value;
	OS_status_t status;
}OS_return_t;

/**
* Functions.
*/
OS_return_t OS_TaskCreate(OS_task_func_t function, const char* task_name, uint8_t default_time_burst, OS_state default_state);

void OS_Tick(void);
void OS_TaskExecution(void);

void OS_PrintTask(uint8_t task_number);

OS_state OS_GetTaskSate(uint8_t task_number);
uint8_t OS_GetTaskBurstTime(uint8_t task_number);
uint8_t OS_GetTaskCntTime(uint8_t task_number);
const char* OS_GetTaskName(uint8_t task_number);
OS_return_t OS_GetTaskByName(char* s_str);

void OS_SetTaskSate(uint8_t task_number, OS_state new_state);
void OS_SetTaskBurstTime(uint8_t task_number, uint8_t new_time_burst);
void OS_SetTaskCntTime(uint8_t task_number, uint8_t new_time_cnt);

#endif /* OS_H_ */