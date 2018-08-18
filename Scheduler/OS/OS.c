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

#include "OS.h"

static OS_struct    task_array[OS_MAX_TASK_NUM];    /**< Variables and information for every single task. */
static uint8_t      task_number = 0u;               /**< Number of the registered tasks. */

/**
 * @brief   This function registers the tasks.
 *          At the beginning there is an error check for registration.
 *          If everything is good, the input values are saved.
 * @param   function: The task we want to call periodically.
 * @param   default_time_burst: The time it gets called periodically.
 * @param   default_state: The state it starts (recommended state: BLOCKED).
 * @return  OS_feedback: Feedback about the success or cause of error of the registration.
 */
OS_feedback OS_TaskCreate(fncPtr function, uint8_t default_time_burst, OS_state default_state)
{
    OS_feedback ret = NOK_UNKNOWN;

    /* Null pointer. */
    if (NULL == task_array)
    {
        ret = NOK_NULL_PTR;
    }
    /* Time limit. */
    else if ((OS_MIN_TIME > default_time_burst) || (OS_MAX_TIME < default_time_burst))
    {
        ret = NOK_TIME_LIMIT;
    }
    /* Task number limit. */
    else if (OS_MAX_TASK_NUM <= task_number)
    {
        ret = NOK_CNT_LIMIT;
    }
    /* Everything is fine, save. */
    else
    {
        task_array[task_number].function = function;
        task_array[task_number].time_burst = default_time_burst;
        task_array[task_number].state = default_state;
        task_array[task_number].time_cnt = 1u;
        task_number++;
        ret = OK;
    }

    return ret;
}

/**
 * @brief   This function keeps track of the tasks' time and puts them into READY state.
 *          This function SHALL be called in a timer interrupt.
 * @param   void
 * @return  void
 */
void OS_TaskTimer(void)
{
    for (uint8_t i = 0u; i < task_number; i++)
    {
        /* Ignore SUSPENDED tasks. */
        if (SUSPENDED != task_array[i].state)
        {
            /* Put it into READY state. */
            if (task_array[i].time_burst <= task_array[i].time_cnt)
            {
                task_array[i].time_cnt = 1u;
                task_array[i].state	= READY;
            }
            /* Or keep counting. */
            else
            {
                task_array[i].time_cnt++;
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
    for (uint8_t i = 0u; i < task_number; i++)
    {
        /* If it is ready, then call it.*/
        if (READY == task_array[i].state)
        {
            task_array[i].function();
            task_array[i].state = BLOCKED;
        }
    }
}

/**
 * @brief   Returns the state of the task.
 * @param   task_number: Which task's state.
 * @return  OS_state: state of the task.
 */
OS_state OS_GetTaskSate(uint8_t task_number)
{
    return task_array[task_number].state;
}

/**
 * @brief   Returns the burst time of the task.
 * @param   task_number: Which task's burst time.
 * @return  The burst time.
 */
uint8_t OS_GetTaskBurstTime(uint8_t task_number)
{
    return task_array[task_number].time_burst;
}

/**
 * @brief   Returns the current counter value of the task.
 * @param   task_number: Which task's counter.
 * @return  The counter.
 */
uint8_t	OS_GetTaskCntTime(uint8_t task_number)
{
    return task_array[task_number].time_cnt;
}

/**
 * @brief   Manually changes the task's state.
 * @param   task_number: Which task's new state.
 * @param   new_state: The new state of the task.
 * @return  void
 */
void OS_SetTaskSate(uint8_t task_number, OS_state new_state)
{
    task_array[task_number].state = new_state;
}

/**
 * @brief   Manually changes the task's burst time.
 * @param   task_number: Which task's new burst time.
 * @param   new_time_burst: The new burst time of the task.
 * @return  void
 */
void OS_SetTaskBurstTime(uint8_t task_number, uint8_t new_time_burst)
{
    task_array[task_number].time_burst = new_time_burst;
}

/**
 * @brief   Manually changes the task's counter.
 * @param   task_number: Which task's new counter value.
 * @param   new_time_cnt: The new counter value of the task.
 * @return  void
 */
void OS_SetTaskCntTime(uint8_t task_number, uint8_t new_time_cnt)
{
    task_array[task_number].time_cnt = new_time_cnt; 
}