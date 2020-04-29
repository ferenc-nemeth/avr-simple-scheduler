/**
* @file    OS.c
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

#include <avr/interrupt.h>
#include "globals.h"
#include "OS.h"

static OS_struct    _tasks[OS_MAX_TASK_NUM];    /**< Variables and information for every single task. */
static uint8_t      _last_task_number = 0u;               /**< Number of the registered tasks. */

/**
* @brief   This function registers the tasks.
*          At the beginning there is an error check for registration.
*          If everything is good, the input values are saved.
* @param   function: The task we want to call periodically.
* @param   default_time_burst: The time it gets called periodically.
* @param   default_state: The state it starts (recommended state: BLOCKED).
* @return  OS_feedback: Feedback about the success or cause of error of the registration.
*/
OS_return_t OS_TaskCreate(OS_task_func_t function, const char* task_name, uint8_t default_time_burst, OS_state default_state)
{
	OS_return_t rtn;
	rtn.status = OS_NOK_UNKNOWN;
	rtn.value = -1;
	
	dbg_puts_P("OS_TaskCreate: ");

	/* Null pointer. */
	if (NULL == _tasks)
	{
		rtn.status = OS_NOK_NULL_PTR;
	}

	#if OS_CHECK_TIME_LIMITS
	/* Time limit. */
	else if ((OS_MIN_TIME > default_time_burst) || (OS_MAX_TIME < default_time_burst))
	{
		rtn.status = OS_NOK_TIME_LIMIT;
	}
	#endif
	/* Task number limit. */
	else if (OS_MAX_TASK_NUM <= _last_task_number)
	{
		rtn.status = OS_NOK_CNT_LIMIT;
	}
	/* Everything is fine, save. */
	else
	{
		_tasks[_last_task_number].function = function;
		_tasks[_last_task_number].task_name = task_name;
		_tasks[_last_task_number].time_burst = default_time_burst;
		_tasks[_last_task_number].state = default_state;
		_tasks[_last_task_number].time_cnt = 1u;
		
		OS_PrintTask(_last_task_number);
		
		rtn.status = OS_OK;
		rtn.value = _last_task_number;
		_last_task_number++;
	}
	
	if(rtn.status != OS_OK) dbg_printf_P(RED("FAILED, reason %d\n"), rtn.status);
	return rtn;
}

void OS_PrintTask(uint8_t task_nr)
{
	dbg_printf_P("Task %d, " GREEN("%s") ", state: %d, cnt/burst: %d/%d\n",
	task_nr,
	_tasks[task_nr].task_name,
	_tasks[task_nr].state,
	_tasks[task_nr].time_cnt,
	_tasks[task_nr].time_burst);
}

/**
* @brief   This function keeps track of the tasks' time and puts them into READY state.
*          This function SHALL be called in a timer interrupt.
* @param   void
* @return  void
*/
void OS_Tick(void)
{
	for (uint8_t i = 0u; i < _last_task_number; i++)
	{
		/* Ignore SUSPENDED tasks. */
		if (SUSPENDED != _tasks[i].state)
		{
			/* Put it into READY state. */
			if (_tasks[i].time_burst <= _tasks[i].time_cnt)
			{
				_tasks[i].time_cnt = 1u;
				_tasks[i].state	= READY;
			}
			/* Or keep counting. */
			else
			{
				_tasks[i].time_cnt++;
			}
		}
	}
}

/**
* @brief   This function calls the READY tasks and then puts them back into BLOCKED state.
*          This function SHALL be called in the infinite loop.
* @param   void
* @return  void
*/
void OS_TaskExecution(void)
{
	for (uint8_t i = 0u; i < _last_task_number; i++)
	{
		/* If it is ready, then call it.*/
		if (READY == _tasks[i].state)
		{
			_tasks[i].state = BLOCKED; // Set before, so state may be overridden from task body (e.g. for one shot functions)
			_tasks[i].function(i);
		}
	}
}

/**
* @brief   Returns the state of the task.
* @param   task_number: Which task's state.
* @return  OS_state: state of the task.
*/
OS_state OS_GetTaskSate(uint8_t task_nr)
{
	return _tasks[task_nr].state;
}

/**
* @brief   Returns the burst time of the task.
* @param   task_number: Which task's burst time.
* @return  The burst time.
*/
uint8_t OS_GetTaskBurstTime(uint8_t task_nr)
{
	return _tasks[task_nr].time_burst;
}

/**
* @brief   Returns the current counter value of the task.
* @param   task_number: Which task's counter.
* @return  The counter.
*/
uint8_t	OS_GetTaskCntTime(uint8_t task_nr)
{
	return _tasks[task_nr].time_cnt;
}

/**
* @brief   Returns the (given) name of the task.
* @param   task_number: Which task's name.
* @return  The name.
*/
const char* OS_GetTaskName(uint8_t task_nr)
{
	return _tasks[task_nr].task_name;
}

/**
* @brief   Finds and returns the task number for a given task name. (relatively slow)
* @param   Searched task name string
* @return  Task number
*/
OS_return_t OS_GetTaskByName(char* s_str)
{
	OS_return_t rtn;
	
	rtn.status = OS_NOT_FOUND;
	rtn.value = -1;
	
	uint8_t search_len = strlen(s_str);
	
	for (uint8_t i = 0u; i < _last_task_number; i++)
	{
		uint8_t task_name_len = strlen(_tasks[i].task_name);
		
		if(task_name_len < search_len ? 0 : strncmp(s_str, _tasks[i].task_name, search_len) == 0)
		{
			rtn.status = OS_OK;
			rtn.value = i;
			break;
		}
	}
	return rtn;
}


/**
* @brief   Manually changes the task's state.
* @param   task_number: Which task's new state.
* @param   new_state: The new state of the task.
* @return  void
*/
void OS_SetTaskSate(uint8_t task_nr, OS_state new_state)
{
	_tasks[task_nr].state = new_state;
}

/**
* @brief   Manually changes the task's burst time.
* @param   task_number: Which task's new burst time.
* @param   new_time_burst: The new burst time of the task.
* @return  void
*/
void OS_SetTaskBurstTime(uint8_t task_nr, uint8_t new_time_burst)
{
	dbg_printf_P("OS_SetTaskBurstTime(task %d, time_burst: %d)\n", task_nr, new_time_burst);
	_tasks[task_nr].time_burst = new_time_burst;
}

/**
* @brief   Manually changes the task's counter.
* @param   task_number: Which task's new counter value.
* @param   new_time_cnt: The new counter value of the task.
* @return  void
*/
void OS_SetTaskCntTime(uint8_t task_nr, uint8_t new_time_cnt)
{
	_tasks[task_nr].time_cnt = new_time_cnt;
}
